#pragma once
#include <string>

namespace GameUtils
{
	// Update .scc to .smd/.scd
	void UpdateSCCSong(const std::string& path);

	// Update .sm to .smd/.scd
	void UpdateSMSong(const std::string& path);
}