#include "AudioMgr.h"
#include "Misc/Requires.h"

AudioManager* AudioManager::mpInstance;

void AudioManager::Initialize(int sampleRate, int channels, int samplesPerBuffer)
{
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = sampleRate;
    desiredSpec.format = AUDIO_F32SYS;
    desiredSpec.channels = channels;
    desiredSpec.samples = samplesPerBuffer;
    desiredSpec.callback = SDLAudioCallback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;
    SDL_zero(obtainedSpec);
    mDeviceID = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);

    Requires(mDeviceID != 0, SDL_GetError());
    SDL_PauseAudioDevice(mDeviceID, 0);
}

void AudioManager::Shutdown()
{
    SDL_CloseAudioDevice(mDeviceID);
}

void AudioManager::RegisterAudio(Audio* audio)
{
    std::lock_guard<std::mutex> lock(mAudioMutex);
    mActiveAudios.push_back(audio);
}

void AudioManager::UnregisterAudio(Audio* audio)
{
    std::lock_guard<std::mutex> lock(mAudioMutex);
    mActiveAudios.erase(std::remove(mActiveAudios.begin(), mActiveAudios.end(), audio), mActiveAudios.end());
}

void AudioManager::SDLAudioCallback(void* userdata, uint8_t* stream, int len)
{
    AudioMgr->MixAudio(stream, len);
}

void AudioManager::MixAudio(uint8_t* stream, int len)
{
    int numSamples = len / sizeof(float);
    float* out = reinterpret_cast<float*>(stream);
    // Clear the buffer.
    std::memset(out, 0, len);

    std::lock_guard<std::mutex> lock(mAudioMutex);
    for (Audio* audio : mActiveAudios) {
        if (!audio->IsPlaying())
            continue;
        // Retrieve a chunk of samples from this audio object.
        std::vector<float> samples = audio->GetSamples(numSamples);
        int samplesCount = static_cast<int>(samples.size());
        // Simple mixing: add each sample into the output.
        // (Note: in production code, check for clipping, summing control, etc.)
        for (int i = 0; i < samplesCount; ++i) {
            out[i] += samples[i];
        }
    }
}


/*
SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = sampleRate;
    desiredSpec.format = AUDIO_F32SYS; // 32-bit float
    desiredSpec.channels = channels;
    desiredSpec.samples = samplesPerBuffer;
    desiredSpec.callback = SDLAudioCallback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;
    SDL_zero(obtainedSpec);
    deviceID = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    if (deviceID == 0) {
        std::cerr << "SDL_OpenAudioDevice Error: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(deviceID, 0);  // Start playback
    return true;
}

void AudioManager::shutdown() {
    SDL_CloseAudioDevice(deviceID);
}

void AudioManager::registerAudio(Audio* audio) {
    std::lock_guard<std::mutex> lock(audioMutex);
    activeAudios.push_back(audio);
}

void AudioManager::unregisterAudio(Audio* audio) {
    std::lock_guard<std::mutex> lock(audioMutex);
    activeAudios.erase(std::remove(activeAudios.begin(), activeAudios.end(), audio), activeAudios.end());
}

void AudioManager::SDLAudioCallback(void* userdata, Uint8* stream, int len) {
    AudioManager* manager = static_cast<AudioManager*>(userdata);
    manager->mixAudio(stream, len);
}

void AudioManager::mixAudio(Uint8* stream, int len) {
    int numSamples = len / sizeof(float);
    float* out = reinterpret_cast<float*>(stream);
    // Clear the buffer.
    std::memset(out, 0, len);

    std::lock_guard<std::mutex> lock(audioMutex);
    for (Audio* audio : activeAudios) {
        if (!audio->isPlaying)
            continue;
        // Retrieve a chunk of samples from this audio object.
        std::vector<float> samples = audio->getSamples(numSamples);
        int samplesCount = static_cast<int>(samples.size());
        // Simple mixing: add each sample into the output.
        // (Note: in production code, check for clipping, summing control, etc.)
        for (int i = 0; i < samplesCount; ++i) {
            out[i] += samples[i];
        }
    }
}

*/


/*
#include "AudioManager.h"
#include "Audio.h"
#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Audio failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    AudioManager audioManager;
    if (!audioManager.init()) {
        std::cerr << "Failed to initialize AudioManager." << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an Audio object that loads its data from a file.
    // Replace "path/to/audio.wav" with the actual file path.
    Audio myAudio("path/to/audio.wav");
    myAudio.Play();

    // Register our audio object with the AudioManager.
    audioManager.registerAudio(&myAudio);

    std::cout << "Playing file-based audio... (waiting 5 seconds)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Clean up.
    audioManager.unregisterAudio(&myAudio);
    audioManager.shutdown();
    SDL_Quit();
    return 0;
}
*/