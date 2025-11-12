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

    GlobalEvents::gOnSongPlay.Add([&](Song* song) { mMusicStartTick = SDL_GetTicks(); });
    GlobalEvents::gOnSongStop.Add([&](Song* song) { mMusicStartTick = 0; });

    mIsCueThreadRunning = true;
    mCueThread = std::thread(&AudioManager::AudioCueLoop, this);
}

void AudioManager::Shutdown()
{
    // SDL_CloseAudioDevice(mDeviceID);

    mIsCueThreadRunning = false;
    mCueCV.notify_all();
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

void AudioManager::QueueSound(Audio* sfx, const f32 time)
{
    if (time <= 0.0f)
        sfx->Play();

    std::lock_guard<std::mutex> lock(mCueMutex);
    mCues.push({sfx, time});
    mCueCV.notify_one();
}

void AudioManager::AudioCueLoop()
{
    // PrintDebug("AudioCueLoop started!");

    std::unique_lock<std::mutex> lock(mCueMutex);
    while (mIsCueThreadRunning)
    {
        if (mCues.empty())
        {
            mCueCV.wait(lock);
            // PrintDebug("mCues size: {}", mCues.size());
        }
        else
        {
            const f32 currTime = GetMusicTime();
            AudioCue cue = mCues.top();
            if (cue.mTime <= currTime)
            {
                mCues.pop();
                cue.mAudio->Play();
                // PrintDebug("Cue played from {} at time {}", cue.mTime, currTime);
            }
            else
            {
                f32 delay = cue.mTime - currTime;
                auto wakeTime = std::chrono::steady_clock::now() + std::chrono::duration<f32>(delay);
                mCueCV.wait_until(lock, wakeTime);
            }
        }
    }
}
