#pragma once
#include "GL/glew.h"

#include <glm.hpp>

#include <vector>
#include <string>
#include <map>

struct Material;

struct ShaderComp
{
	ShaderComp(const GLenum type, const std::string& file);

	GLuint mShaderID;
	GLenum mShaderType;
	
	const std::string mFile;
};

struct Shader
{
	Shader(const std::vector<ShaderComp>& comps);

	template <class T, class... Ts>
	Shader(T fb, Ts... rest) : Shader(InitializeShader(fb, rest...)) {}
	Shader(const std::string& shaderFile) : Shader(InitializeShader(shaderFile)) {}

	~Shader();

public:
	void Bind() const { glUseProgram(mID); }

#pragma region SetUniform
	inline void SetUniform(const GLint loc, const bool& val) const noexcept
	{
		glUniform1i(loc, static_cast<int>(val));
	}
	inline void SetUniform(const GLint loc, const int& val) const noexcept
	{
		glUniform1i(loc, val);
	}
	inline void SetUniform(const GLint loc, const GLuint& val) const noexcept
	{
		glUniform1i(loc, val);
	}
	inline void SetUniform(const GLint loc, const float& val) const noexcept
	{
		glUniform1f(loc, val);
	}
	inline void SetUniform(const GLint loc, const glm::vec2& vec) const noexcept
	{
		glUniform2f(loc, vec.x, vec.y);
	}
	inline void SetUniform(const GLint loc, const glm::vec3& vec) const noexcept
	{
		glUniform3f(loc, vec.x, vec.y, vec.z);
	}
	inline void SetUniform(const GLint loc, const glm::vec4& vec) const noexcept
	{
		glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
	}
	inline void SetUniform(const GLint loc, const glm::mat3& mat) const noexcept
	{
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	inline void SetUniform(const GLint loc, const glm::mat4& mat) const noexcept
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
	}

	template <typename T>
	void SetUniform(const std::string& name, const std::vector<T>& vector)
	{
		for (size_t i = 0; i < vector.size(); i++)
		{
			SetUniform(name + "[" + std::to_string(i) + "]", vector[i]);
		}
	}

	template <typename T, size_t SIZE>
	void SetUniform(const std::string& name, const std::array<T, SIZE>& array)
	{
		for (size_t i = 0; i < SIZE; i++)
		{
			SetUniform(name + "[" + std::to_string(i) + "]", array[i]);
		}
	}

	template <typename T>
	inline void SetUniform(const std::string& name, const T& val) noexcept
	{
		auto it = mUniformLocations.find(name);
		if (it != mUniformLocations.end())
		{
			SetUniform(it->second, val);
		}
		GLint uniformLoc = glGetUniformLocation(mID, name.c_str());
		if (uniformLoc != -1)
		{
			SetUniform(uniformLoc, val);
			mUniformLocations[name] = uniformLoc;
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

private:
	std::map<std::string, GLint> mUniformLocations;
};

template<class T, class ...Ts>
inline std::vector<ShaderComp> Shader::InitializeShader(T shader, Ts ...rest) const
{
	std::vector<ShaderComp> shaderList = { shader };
	shaderList.insert(shaderList.end(), InitializeShader(rest...));
	return shaderList;
}

