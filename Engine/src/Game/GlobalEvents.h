#pragma once
#include "Composition/Events/Event.h"
#include "Chart.h"
#include "Song.h"

CREATE_EVENT(OnSongPlay);
CREATE_EVENT(OnBeatTick);

CREATE_MULTICAST_EVENT(OnSongCreate, Song*);
CREATE_MULTICAST_EVENT(OnSongUpdate, float);
CREATE_MULTICAST_EVENT(OnChartEdit, Chart*);

namespace GlobalEvents
{
	extern OnSongPlay gOnSongPlay;
	extern OnBeatTick gOnBeatTick;

	extern OnSongCreate gOnSongCreate;
	extern OnSongUpdate gOnSongUpdate;
	extern OnChartEdit gOnChartEdit;

}