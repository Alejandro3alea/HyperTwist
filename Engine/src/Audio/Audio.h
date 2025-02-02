#pragma once
#include "Resources/ResourceMgr.h"
#include "SFML/Audio.hpp"

struct Audio
{
    Audio(const std::string& file);

    void Play(const float positionInMeasure);
    void Play();
    void Stop();

    // Hypertwist
    void SetPosition(const float measure);
    void SetVolume(const float val);

private:
    sf::Music mAudio;
};