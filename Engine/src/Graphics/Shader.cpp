#include "Shader.h"
#include "Misc/ColorConsole.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

ShaderComp::ShaderComp(const GLenum type, const std::string& file) : mFile(file), mShaderType(type)
{
	// Create shader
	mShaderID = glCreateShader(type);

	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
	{
		PrintWarning("File not read in " + file);
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

		PrintWarning("Compile failure in " + file + "\n" + strInfoLog);
		delete[] strInfoLog;
	}
	else
	{
		PrintSuccess("Shader " + file + " successfully read");
	}
}

Shader::Shader(const std::vector<ShaderComp>& comps)
{
	mID = glCreateProgram();

	for (auto it : comps)
	{
		glAttachShader(mID, it.mShaderID);
	}

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

		std::string linkerFailureStr = std::string("Linker failure with...");
		for (auto it : comps)
		{
			linkerFailureStr += it.mFile + "\n";
		}
		linkerFailureStr += std::string(": \n") + strInfoLog;
		PrintWarning(linkerFailureStr);
		delete[] strInfoLog;
	}

	for (auto it : comps)
		glDetachShader(mID, it.mShaderID);
}

Shader::~Shader()
{
	PrintDebug("Deleted Shader.");
}

std::vector<ShaderComp> Shader::InitializeShader(const std::string& file) const
{
	std::filesystem::path basePath = file;
	basePath = basePath.parent_path();

	std::vector<ShaderComp> comps;

	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
		PrintWarning("File not read in " + file);

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

		std::filesystem::path relativePath = shaderPath;
		std::filesystem::path fullPath = basePath / relativePath;
		comps.push_back(ShaderComp(shaderType, fullPath.string()));
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