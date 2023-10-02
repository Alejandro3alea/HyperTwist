#include "ImGuiWidgets.h"
#include "Editor.h"

#include <functional>

namespace ImGui
{
	const unsigned MaxCharCount = 256;


	bool OnGuiBase(const bool check, const std::function<void()>& then)
	{
		bool changed = false;
		if (check)
		{
			changed = true;
			then();
		}

		return changed;
	}

	bool OnGui(const std::string& label, bool& val)
	{
		return OnGuiBase(ImGui::Checkbox((label).c_str(), &val), []() {});
	}

	bool OnGui(const std::string& label, int& val)
	{
		return OnGuiBase(ImGui::SliderInt((label).c_str(), &val, 0, 5000), []() {});
	}

	bool OnGui(const std::string& label, unsigned& val)
	{
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt((label).c_str(), &intVal, 0, 5000), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, short& val)
	{
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt((label).c_str(), &intVal, 0, 5000), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, long& val)
	{
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt((label).c_str(), &intVal, 0, 5000), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, float& val, const float rate, const float min, const float max)
	{
		return OnGuiBase(ImGui::DragFloat((label).c_str(), &val, rate, min, max), []() {});
	}

	bool OnGui(const std::string& label, double& val)
	{
		return OnGuiBase(ImGui::InputDouble((label).c_str(), &val), []() {});
	}

	bool OnGui(const std::string& label, std::string& val)
	{
		char c[MaxCharCount];
		for (unsigned i = 0; i < val.size(); i++)
			c[i] = val[i];
		c[val.size()] = '\0';
		return OnGuiBase(ImGui::InputText((label).c_str(), c, MaxCharCount), [&val, c]() { val = std::string(c); });
	}

	bool OnGui(const std::string& label, glm::vec4& val)
	{
		return OnGuiBase(ImGui::DragFloat4((label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::vec3& val)
	{
		return OnGuiBase(ImGui::DragFloat3((label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::vec2& val)
	{
		return OnGuiBase(ImGui::DragFloat2((label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::uvec4& val)
	{
		glm::ivec4 intVal = static_cast<glm::ivec4>(val);
		return OnGuiBase(ImGui::DragInt4((label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec4>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::uvec3& val)
	{
		glm::ivec3 intVal = static_cast<glm::ivec3>(val);
		return OnGuiBase(ImGui::DragInt3((label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec3>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::uvec2& val)
	{
		glm::ivec2 intVal = static_cast<glm::ivec2>(val);
		return OnGuiBase(ImGui::DragInt2((label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec2>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::ivec4& val)
	{
		return OnGuiBase(ImGui::DragInt4((label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::ivec3& val)
	{
		return OnGuiBase(ImGui::DragInt4((label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::ivec2& val)
	{
		return OnGuiBase(ImGui::DragInt4((label).c_str(), &val[0], 0.01f), []() {});
	}
}