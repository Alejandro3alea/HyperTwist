#pragma once
#include "Game/Chart.h"
#include <string>

namespace GameUtils
{
	// Update .scc to .smd/.scd
	void UpdateSCCSongToSMD(const std::string& path);

	// Update .sm to .smd/.scd
	void UpdateSMSongToSMD(const std::string& path);


	std::string ChartDifficultyToStr(const ChartDifficulty& val);
	ChartDifficulty StrToChartDifficulty(const std::string& str);
}