#pragma once
#include "GL/glew.h"

#include <glm.hpp>

#include <vector>
#include <string>

struct Material;

struct ShaderComp
{
	ShaderComp(const GLenum type, const std::string& file);

	GLuint mShaderID;
	GLenum mShaderType;
};

struct Shader
{
	Shader(const std::vector<ShaderComp>& comps);

	template <class T, class... Ts>
	Shader(T fb, Ts... rest) : Shader(InitializeShader(fb, rest)) {}
	Shader(const std::string& shaderFile) : Shader(InitializeShader(shaderFile)) {}

	~Shader();

public:
	void Bind() { glUseProgram(mID); }

#pragma region SetUniform
	void SetUniform(const std::string& name, const bool val) const noexcept;
	void SetUniform(const std::string& name, const int val) const noexcept;
	void SetUniform(const std::string& name, const GLuint val) const noexcept;
	void SetUniform(const std::string& name, const float val) const noexcept;
	void SetUniform(const std::string& name, const glm::vec2& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::vec3& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::vec4& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::mat3& mat) const noexcept;
	void SetUniform(const std::string& name, const glm::mat4& mat) const noexcept;

	void SetUniform(const GLuint loc, const bool val) const noexcept;
	void SetUniform(const GLuint loc, const int val) const noexcept;
	void SetUniform(const GLuint loc, const GLuint val) const noexcept;
	void SetUniform(const GLuint loc, const float val) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec2& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec3& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec4& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::mat3& mat) const noexcept;
	void SetUniform(const GLuint loc, const glm::mat4& mat) const noexcept;

	template <typename T>
	void SetUniform(const std::string& name, std::vector<T> vector)
	{
		for (size_t i = 0; i < vector.size(); i++)
		{
			SetUniform(name + "[" + std::to_string(i) + "]", vector[i]);
		}
	}
#pragma endregion

protected:
	template <class T, class... Ts>
	std::vector<ShaderComp> InitializeShader(T fb, Ts... rest) const;
	std::vector<ShaderComp> InitializeShader(const std::string& file) const;

	GLenum CheckShaderCompType(const std::string& val) const;

//--------------------------------------------------------------------
public:
	GLuint mID;
};

template<class T, class ...Ts>
inline std::vector<ShaderComp> Shader::InitializeShader(T shader, Ts ...rest) const
{
	std::vector<ShaderComp> shaderList;
	shaderList.push_back();
	shaderList.insert(shaderList.end(), InitializeShader(rest...));
	return shaderList;
}

