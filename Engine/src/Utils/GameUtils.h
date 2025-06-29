#pragma once
#include "Game/Chart.h"
#include "Game/Grade.h"
#include <string>

namespace GameUtils
{
	// Update cong (.sm or .ssc) to .smd/.scd
	void UpdateSongToSMD(const std::string& path);

	std::string ChartDifficultyToStr(const ChartDifficulty& val);
	ChartDifficulty StrToChartDifficulty(const std::string& str);

	Grade ScoreToGrade(const i32 score, const bool fail, const bool fullCombo);
	std::string GradeToStr(const Grade &grade);
	Resource<Texture>* GradeToTexture(const Grade& grade);

	std::string GetBPMLabel(const std::map<float, float>& bpms);
}