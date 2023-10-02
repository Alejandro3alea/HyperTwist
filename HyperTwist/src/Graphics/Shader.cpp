#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

ShaderComp::ShaderComp(const GLenum type, const std::string& file) : mShaderType(type)
{
	// Create shader
	mShaderID = glCreateShader(type);

	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
	{
		std::cout << "[ERROR] File not read in " << file << std::endl;
		return;
	}

	std::stringstream shaderCode;
	char line[256];
	while (tFile.getline(line, 256))
		shaderCode << line << std::endl;

	std::string finalCode = shaderCode.str();

	const char* strFileData = finalCode.c_str();
	glShaderSource(mShaderID, 1, &strFileData, NULL);

	glCompileShader(mShaderID);

	GLint status;
	glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(mShaderID, infoLogLength, NULL, strInfoLog);

		// Get shader type
		const char* strShaderType = NULL;
		switch (mShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		std::cout << "Compile failure in " << strShaderType << " shader: " << std::endl << strInfoLog << std::endl;
		delete[] strInfoLog;
	}
	else
	{
		std::cout << "Shader " << file << " successfully read" << std::endl;
	}
}

Shader::Shader(const std::vector<ShaderComp>& comps)
{
	mID = glCreateProgram();

	for (auto it : comps)
		glAttachShader(mID, it.mShaderID);

	glLinkProgram(mID);


	GLint status;
	glGetProgramiv(mID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &infoLogLength);

		int infoSize = infoLogLength + 1;
		GLchar* strInfoLog = new GLchar[static_cast<size_t>(infoSize)];
		glGetProgramInfoLog(mID, infoLogLength, NULL, strInfoLog);
		std::cout << "Linker failure: " << strInfoLog << std::endl;
		delete[] strInfoLog;
	}

	for (auto it : comps)
		glDetachShader(mID, it.mShaderID);
}

Shader::~Shader()
{
	std::cout << "[DEBUG] Deleted Shader." << std::endl;
}

std::vector<ShaderComp> Shader::InitializeShader(const std::string& file) const
{
	std::vector<ShaderComp> comps;

	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
		std::cout << "[ERROR] File not read in " << file << std::endl;

	char line[256];
	while (tFile.getline(line, 256))
	{
		std::stringstream sShaderType;
		sShaderType << line << std::endl;
		std::string strShaderType = sShaderType.str();
		GLenum shaderType = CheckShaderCompType(strShaderType);

		if (!tFile.getline(line, 256))
			break;

		std::stringstream sShaderFile;
		sShaderFile << line << std::endl;
		std::string shaderPath = sShaderFile.str();
		shaderPath.pop_back();
		comps.push_back(ShaderComp(shaderType, shaderPath));
	}
	
	return comps;
}

GLenum Shader::CheckShaderCompType(const std::string& val) const
{
	if (val.find("VERTEX_SHADER:") != std::string::npos)
		return GL_VERTEX_SHADER;
	else if (val.find("FRAGMENT_SHADER:") != std::string::npos)
		return GL_FRAGMENT_SHADER;
	else if (val.find("GEOMETRY_SHADER:") != std::string::npos)
		return GL_GEOMETRY_SHADER;
	else if (val.find("TESS_CONTROL_SHADER:") != std::string::npos)
		return GL_TESS_CONTROL_SHADER;
	else if (val.find("TESS_EVALUATION_SHADER:") != std::string::npos)
		return GL_TESS_EVALUATION_SHADER;

	return GL_NONE;
}

#pragma region SetUniform
// NAMES
void Shader::SetUniform(const std::string& name, const bool val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), static_cast<int>(val));
}

void Shader::SetUniform(const std::string& name, const int val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
}

void Shader::SetUniform(const std::string& name, const GLuint val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
}

void Shader::SetUniform(const std::string& name, const float val) const noexcept
{
	glUniform1f(glGetUniformLocation(mID, name.c_str()), val);
}

void Shader::SetUniform(const std::string& name, const glm::vec2& vec) const noexcept
{
	glUniform2f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y);
}

void Shader::SetUniform(const std::string& name, const glm::vec3& vec) const noexcept
{
	glUniform3f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const std::string& name, const glm::vec4& vec) const noexcept
{
	glUniform4f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform(const std::string& name, const glm::mat3& mat) const noexcept
{
	glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform(const std::string& name, const glm::mat4& mat) const noexcept
{
	glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

///////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////

// LOCATIONS
void Shader::SetUniform(const GLuint loc, const bool val) const noexcept
{
	glUniform1i(loc, static_cast<int>(val));
}

void Shader::SetUniform(const GLuint loc, const int val) const noexcept
{
	glUniform1i(loc, val);
}

void Shader::SetUniform(const GLuint loc, const GLuint val) const noexcept
{
	glUniform1i(loc, val);
}

void Shader::SetUniform(const GLuint loc, const float val) const noexcept
{
	glUniform1f(loc, val);
}

void Shader::SetUniform(const GLuint loc, const glm::vec2& vec) const noexcept
{
	glUniform2f(loc, vec.x, vec.y);
}

void Shader::SetUniform(const GLuint loc, const glm::vec3& vec) const noexcept
{
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const GLuint loc, const glm::vec4& vec) const noexcept
{
	glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform(const GLuint loc, const glm::mat3& mat) const noexcept
{
	glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform(const GLuint loc, const glm::mat4& mat) const noexcept
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

#pragma endregion