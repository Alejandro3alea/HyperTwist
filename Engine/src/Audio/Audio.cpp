#include "Audio/Audio.h"
#include "Misc/Requires.h"

#include <SDL2/SDL_audio.h>

Audio::Audio(const std::string& file)
{       
    SDL_AudioSpec wavSpec;
    Uint8* wavBuffer = nullptr;
    Uint32 wavLength = 0;

    if (SDL_LoadWAV(file.c_str(), &wavSpec, &wavBuffer, &wavLength) == nullptr) {
        std::cerr << "Failed to load WAV file: " << file << " Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = mSampleRate;
    desiredSpec.format = AUDIO_F32SYS;
    desiredSpec.channels = 2;
    desiredSpec.samples = 256;

    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, wavSpec.format, wavSpec.channels, wavSpec.freq,
        desiredSpec.format, desiredSpec.channels, desiredSpec.freq) < 0) {
        std::cerr << "Failed to build audio converter: " << SDL_GetError() << std::endl;
        SDL_FreeWAV(wavBuffer);
        return;
    }

    cvt.len = wavLength;
    cvt.buf = (Uint8*)SDL_malloc(cvt.len * cvt.len_mult);
    if (!cvt.buf) {
        std::cerr << "Failed to allocate memory for audio conversion" << std::endl;
        SDL_FreeWAV(wavBuffer);
        return;
    }

    memcpy(cvt.buf, wavBuffer, wavLength);
    SDL_FreeWAV(wavBuffer);

    if (SDL_ConvertAudio(&cvt) < 0) {
        std::cerr << "Audio conversion failed: " << SDL_GetError() << std::endl;
        SDL_free(cvt.buf);
        return;
    }

    size_t numSamples = cvt.len_cvt / sizeof(float);
    mSamples.resize(numSamples);
    memcpy(mSamples.data(), cvt.buf, cvt.len_cvt);
    SDL_free(cvt.buf);
}

void Audio::Play(const float positionInMeasure)
{
    //mAudio.play();
    SetPosition(positionInMeasure);
    mIsPlaying = true;
}

void Audio::Play()
{
    //mAudio.play();
    mIsPlaying = true;
}

void Audio::Stop()
{
    //mAudio.stop();
    mIsPlaying = false;
}


void Audio::SetPosition(const float measure)
{
   //mAudio.setPlayingOffset(sf::seconds(measure));
}

void Audio::SetVolume(const float val)
{
    mVolume = std::clamp(val, 0.0f, 1.0f);
}

std::vector<float> Audio::GetSamples(const size_t numSamples)
{
    if (!mIsPlaying)
        return std::vector<float>();

    std::vector<float> result(numSamples);
    for (size_t i = 0; i < numSamples; ++i)
    {
        if (mCurrSample < mSamples.size())
        {
            result[i] = mSamples[mCurrSample] * mVolume;
            mCurrSample++;
        }
        else // End of audio
        {
            mIsPlaying = false;
            break;
        }
    }

    return result;
}

void Audio::Cleanup()
{
    // No memory to cleanup
    if (!IsPlaying())
        return;

    i64 currentHardwareFrame = AudioMgr->GetPosition(nullptr);

    mMutex.lock();


    mMutex.unlock();
}


AudioInfo LoadWav(const std::string& path)
{
    SDL_AudioSpec spec;
    Uint8* buf;
    Uint32 len;
    if (SDL_LoadWAV(path.c_str(), &spec, &buf, &len) == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    AudioInfo data;
    data.sampleRate = spec.freq;
    data.channels = spec.channels;
    data.pcm.assign(reinterpret_cast<int16_t*>(buf),
        reinterpret_cast<int16_t*>(buf + len));
    SDL_FreeWAV(buf);
    return data;
}