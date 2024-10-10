#include "Audio/Audio.h"
#include "Misc/Ensure.h"

Audio::Audio(const std::string& file)
{
    Ensure(mAudio.openFromFile(file), "Error loading the audio file.");
}

void Audio::Play()
{
    mAudio.play();
}


void Audio::SetPosition(const float measure)
{
    mAudio.setPlayingOffset(sf::seconds(measure));
}

void Audio::SetVolume(const float val)
{
    mAudio.setVolume(val);
}
