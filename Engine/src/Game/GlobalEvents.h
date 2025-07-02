#pragma once
#include "Composition/Events/Event.h"
#include "Chart.h"
#include "Song.h"

CREATE_EVENT(OnBeatTick);

CREATE_MULTICAST_EVENT(OnSongPlay, Song*);
CREATE_MULTICAST_EVENT(OnSongPause, Song*);
CREATE_MULTICAST_EVENT(OnSongStop, Song*);

CREATE_MULTICAST_EVENT(OnSongCreate, Song*);
CREATE_MULTICAST_EVENT(OnSongUpdate, float);
CREATE_MULTICAST_EVENT(OnChartEdit, Chart*);

namespace GlobalEvents
{
	extern OnBeatTick gOnBeatTick;

	extern OnSongPlay gOnSongPlay;
	extern OnSongPause gOnSongPause;
	extern OnSongStop gOnSongStop;

	extern OnSongCreate gOnSongCreate;
	extern OnSongUpdate gOnSongUpdate;
	extern OnChartEdit gOnChartEdit;

}