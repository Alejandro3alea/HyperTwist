#pragma once
#include "Game/Chart.h"
#include <string>

namespace GameUtils
{
	// Update cong (.sm or .ssc) to .smd/.scd
	void UpdateSongToSMD(const std::string& path);

	std::string ChartDifficultyToStr(const ChartDifficulty& val);
	ChartDifficulty StrToChartDifficulty(const std::string& str);
}