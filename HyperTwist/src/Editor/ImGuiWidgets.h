#pragma once
#include "ResourceMgr.h"

#include <glm.hpp>
#include <string>

namespace ImGui
{
	bool OnGui(const std::string& label, bool& val);
	bool OnGui(const std::string& label, int& val);
	bool OnGui(const std::string& label, unsigned& val);
	bool OnGui(const std::string& label, short& val);
	bool OnGui(const std::string& label, long& val);
	bool OnGui(const std::string& label, float& val, const float rate = 0.01f, const float min = -FLT_MAX, const float max = FLT_MAX);
	bool OnGui(const std::string& label, double& val);

	bool OnGui(const std::string& label, std::string& val);

	bool OnGui(const std::string& label, glm::vec4& val);
	bool OnGui(const std::string& label, glm::vec3& val);
	bool OnGui(const std::string& label, glm::vec2& val);
	bool OnGui(const std::string& label, glm::uvec4& val);
	bool OnGui(const std::string& label, glm::uvec3& val);
	bool OnGui(const std::string& label, glm::uvec2& val);
	bool OnGui(const std::string& label, glm::ivec4& val);
	bool OnGui(const std::string& label, glm::ivec3& val);
	bool OnGui(const std::string& label, glm::ivec2& val);
}