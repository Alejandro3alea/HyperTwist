#pragma once
#include "Requires.h"

#include <json.hpp>
#include <detail/type_quat.hpp>
#include <glm/glm.hpp>

using json = nlohmann::json;

#pragma region FROM_JSON
#pragma region DEFAULT_DATATYPES
inline json& operator>>(json& val, int8_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<int8_t>();
	return val;
}

inline json& operator>>(json& val, uint8_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<uint8_t>();
	return val;
}

inline json& operator>>(json& val, int16_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<int16_t>();
	return val;
}

inline json& operator>>(json& val, uint16_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<uint16_t>();
	return val;
}

inline json& operator>>(json& val, int32_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<int32_t>();
	return val;
}

inline json& operator>>(json& val, uint32_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<uint32_t>();
	return val;
}

inline json& operator>>(json& val, int64_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<int64_t>();
	return val;
}

inline json& operator>>(json& val, uint64_t& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<uint64_t>();
	return val;
}

inline json& operator>>(json& val, float& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<float>();
	return val;
}

inline json& operator>>(json& val, double& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<double>();
	return val;
}

inline json& operator>>(json& val, bool& type)
{
	Requires(val != nullptr, "Json value is nullptr");
	type = val.get<bool>();
	return val;
}
#pragma endregion

inline json& operator>>(json& val, const char*& string)
{
	Requires(val != nullptr, "Json value is nullptr");
	string = val.get<std::string>().c_str();
	return val;
}

inline json& operator>>(json& val, std::string& string)
{
	Requires(val != nullptr, "Json value is nullptr");
	string = val.get<std::string>();
	return val;
}

inline json& operator>>(json& val, glm::vec2& vec)
{
	Requires(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<float>();
	vec.y = val["y"].get<float>();
	return val;
}

inline json& operator>>(json& val, glm::vec3& vec)
{
	Requires(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<float>();
	vec.y = val["y"].get<float>();
	vec.z = val["z"].get<float>();
	return val;
}

inline json& operator>>(json& val, glm::vec4& col)
{
	Requires(val != nullptr, "Json value is nullptr");
	col.r = val["r"].get<float>();
	col.g = val["g"].get<float>();
	col.b = val["b"].get<float>();
	col.a = val["a"].get<float>();
	return val;
}

inline json& operator>>(json& val, glm::quat& quat)
{
	Requires(val != nullptr, "Json value is nullptr");
	quat.x = val["x"].get<float>();
	quat.y = val["y"].get<float>();
	quat.z = val["z"].get<float>();
	quat.w = val["w"].get<float>();
	return val;
}

template <typename T>
json& operator<<(T& type, json& val)
{
	return val >> type;
}
#pragma endregion

#pragma region TO_JSON
#pragma region DEFAULT_DATATYPES
inline json& operator<<(json& val, const int8_t type) { return val = type; }
inline json& operator<<(json& val, const uint8_t type) { return val = type; }
inline json& operator<<(json& val, const int16_t type) { return val = type; }
inline json& operator<<(json& val, const uint16_t type) { return val = type; }
inline json& operator<<(json& val, const int32_t type) { return val = type; }
inline json& operator<<(json& val, const uint32_t type) { return val = type; }
inline json& operator<<(json& val, const int64_t type) { return val = type; }
inline json& operator<<(json& val, const uint64_t type) { return val = type; }
inline json& operator<<(json& val, const float type) { return val = type; }
inline json& operator<<(json& val, const double type) { return val = type; }
inline json& operator<<(json& val, const bool type) { return val = type; }
#pragma endregion

inline json& operator<<(json& val, const char* string) { return val = string; }
inline json& operator<<(json& val, const std::string& string) { return val = string; }

inline json& operator<<(json& val, const glm::vec2& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	val = tVal;
	return val;
}

inline json& operator<<(json& val, const glm::vec3& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	tVal["z"] = vec.z;
	val = tVal;
	return val;
}

inline json& operator<<(json& val, const glm::vec4& col)
{
	json tVal;
	tVal["r"] = col.r;
	tVal["g"] = col.g;
	tVal["b"] = col.b;
	tVal["a"] = col.a;
	val = tVal;
	return val;
}

inline json& operator<<(json& val, glm::quat& quat)
{
	json tVal;
	tVal["x"] = quat.x;
	tVal["y"] = quat.y;
	tVal["z"] = quat.z;
	tVal["w"] = quat.w;
	val = tVal;
	return val;
}

template <typename T>
json& operator>>(T& type, json& val)
{
	return val << type;
}

#pragma endregion