#include "Audio/Audio.h"
#include "Misc/Ensure.h"

Audio::Audio(const std::string& file)
{
    if (mAudio.openFromFile(file))
    {
        PrintWarning("Error loading the audio file: " + ResourceMgr->GetResourceName(file));
    }
}

void Audio::Play(const float positionInMeasure)
{
    mAudio.play();
    SetPosition(positionInMeasure);
}

void Audio::Play()
{
    mAudio.play();
}

void Audio::Stop()
{
    mAudio.stop();
}


void Audio::SetPosition(const float measure)
{
    mAudio.setPlayingOffset(sf::seconds(measure));
}

void Audio::SetVolume(const float val)
{
    mAudio.setVolume(val);
}
