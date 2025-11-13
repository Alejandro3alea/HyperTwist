#include "AudioMgr.h"
#include "Audio.h"
#include "Game/GlobalEvents.h"
#include "Misc/DataTypes.h"
#include "Misc/Requires.h"
#include "Resources/ResourceMgr.h"

#include <SDL3_mixer/SDL_mixer.h>

AudioManager* AudioManager::mpInstance;

void AudioManager::Initialize(int sampleRate, int channels)
{
    Requires(SDL_InitSubSystem(SDL_INIT_AUDIO),
             "Fail initializing SDL_AUDIO subsystem: " + std::string(SDL_GetError()));

    MIX_InitFlags mixerFlags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG;
    Requires(Mix_Init(mixerFlags) == mixerFlags, "Fail initializing SDL_mixer: " + std::string(SDL_GetError()));

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = sampleRate;
    desiredSpec.format = MIX_DEFAULT_FORMAT;
    desiredSpec.channels = channels;
    Requires(Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredSpec),
             "Fail trying to open the default audio device: " + std::string(SDL_GetError()));

    Mix_AllocateChannels(32);

    mBeatTick = ResourceMgr->Load<Audio>("engine/audio/sfx/BeatTick.wav", AudioType::SFX);
    mNoteTick = ResourceMgr->Load<Audio>("engine/audio/sfx/NoteTick.wav", AudioType::SFX);

    GlobalEvents::gOnSongPlay.Add([&](Song* song) { SetupMusicStart(); });
    GlobalEvents::gOnSongStop.Add([&](Song* song) { SetupMusicStop(); });

    mIsCueMusicThreadRunning = true;
    mIsCueTimerThreadRunning = true;
    mIsCueTimerThreadPaused = false;
    mCueSongThread = std::thread(&AudioManager::AudioCueMusicLoop, this);
    mCueTimerThread = std::thread(&AudioManager::AudioCueTimerLoop, this);
}

void AudioManager::Shutdown()
{
    // SDL_CloseAudioDevice(mDeviceID);

    mIsCueMusicThreadRunning = false;
    mCueMusicCV.notify_all();
}

Audio* AudioManager::LoadSound(const std::string& filePath)
{
    Resource<Audio>* sound = ResourceMgr->Load<Audio>(filePath, AudioType::SFX);
    return sound->get();
}

Audio* AudioManager::LoadMusic(const std::string& filePath)
{
    Resource<Audio>* music = ResourceMgr->Load<Audio>(filePath, AudioType::BGM);
    return music->get();
}

f32 AudioManager::GetMusicTime() { return mMusicStartTick ? (SDL_GetTicks() - mMusicStartTick) / 1000.0f : 0.0f; }

void AudioManager::QueueAudioAfter(const Resource<Audio>& audio, const f32 seconds)
{
    if (seconds <= 0.0f)
    {
        audio->Play();
        return;
    }

    std::lock_guard<std::mutex> lock(mCueTimerMutex);
    mTimerCues.push({audio, mQueueTimer + seconds});

    bool should_notify = mTimerCues.empty() || seconds + mQueueTimer < mTimerCues.top().mTime;
    mCueTimerCV.notify_one();
}

void AudioManager::QueueSoundAtTimestamp(const Resource<Audio>& sfx, const f32 music_timestamp_seconds)
{
    if (music_timestamp_seconds <= 0.0f)
    {
        sfx->Play();
        return;
    }

    std::lock_guard<std::mutex> lock(mCueMusicMutex);
    mSongSyncCues.push({sfx, music_timestamp_seconds});
    mCueMusicCV.notify_one();
}

void AudioManager::StopTimerQueue()
{
    std::lock_guard<std::mutex> lock(mCueTimerMutex);
    mIsCueTimerThreadPaused = true;
}

void AudioManager::ResumeTimerQueue()
{
    {
        std::lock_guard<std::mutex> lock(mCueTimerMutex);
        mIsCueTimerThreadPaused = false;
    }

    // Wake up thread
    mCueTimerCV.notify_one();
}

void AudioManager::SetupMusicStart() { mMusicStartTick = SDL_GetTicks(); }

void AudioManager::SetupMusicStop() { mMusicStartTick = 0; }

void AudioManager::AudioCueMusicLoop()
{
    PrintDebug("AudioCueMusicLoop started!");

    try
    {
        std::unique_lock<std::mutex> lock(mCueMusicMutex);
        while (mIsCueMusicThreadRunning)
        {
            // Song sync cues
            if (mSongSyncCues.empty())
            {
                mCueMusicCV.wait(lock);
                // PrintDebug("mCues size: {}", mCues.size());
            }
            else
            {
                const f32 currTime = GetMusicTime();
                AudioCue cue = mSongSyncCues.top();
                if (cue.mTime <= currTime)
                {
                    mSongSyncCues.pop();
                    cue.mAudio->Play();
                    // PrintDebug("Cue played from {} at time {}", cue.mTime, currTime);
                }
                else
                {
                    f32 delay = cue.mTime - currTime;
                    auto wakeTime = std::chrono::steady_clock::now() + std::chrono::duration<f32>(delay);
                    mCueMusicCV.wait_until(lock, wakeTime);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        PrintError("AudioCueMusicLoop crashed with exception: " + std::string(e.what()));
    }
}

void AudioManager::AudioCueTimerLoop()
{
    PrintDebug("AudioCueTimerLoop started!");
    auto last = std::chrono::steady_clock::now();

    try
    {
        std::unique_lock<std::mutex> lock(mCueTimerMutex);
        while (mIsCueTimerThreadRunning)
        {
            if (mIsCueTimerThreadPaused)
            {
                mCueTimerCV.wait(lock, [this] { return !mIsCueTimerThreadPaused || !mIsCueTimerThreadRunning; });
                last = std::chrono::steady_clock::now();
                continue;
            }

            // Timer cues
            if (mTimerCues.empty())
            {
                mQueueTimer = 0.0f;
                mCueTimerCV.wait(lock); // waits until QueueAudioAfter notifies
                last = std::chrono::steady_clock::now();
            }
            else
            {
                auto now = std::chrono::steady_clock::now();
                const float deltaTime = std::chrono::duration<float>(now - last).count();
                last = now;

                mQueueTimer += deltaTime;
                AudioCue cue = mTimerCues.top();
                if (cue.mTime <= mQueueTimer)
                {
                    mTimerCues.pop();
                    PrintDebug("Cue played from {} at time {}", cue.mTime, mQueueTimer);
                    if (cue.mAudio)
                        cue.mAudio->Play();
                    else
                        PrintError("Invalid audio cue sent to AudioCueTimerLoop thread");
                }
                else
                {
                    auto wakeTime =
                        std::chrono::steady_clock::now() + std::chrono::duration<float>(cue.mTime - mQueueTimer);
                    mCueTimerCV.wait_until(lock, wakeTime);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        PrintError("AudioCueTimerLoop crashed with exception: " + std::string(e.what()));
    }
}
