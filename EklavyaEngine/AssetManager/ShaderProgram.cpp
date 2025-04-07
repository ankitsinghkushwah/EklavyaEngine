#include <AssetManager/ShaderProgram.h>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>

using namespace Eklavya::Asset;

ShaderProgram::ShaderProgram(const std::string& assetName) : IAsset(EType::SHADER, assetName), m_IsReadyToUse(false), m_ErrorInfo("N/A")
{
	m_ShaderIDs.reserve(4);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_ID);
}

std::string ShaderProgram::Load(const std::string& path)
{
	FILE* file = fopen(path.c_str(), "r");
	if (file != NULL)
	{
		char        ch = ' ';
		std::string buffer;
		buffer.reserve(1024);

		while (true)
		{
			ch = fgetc(file);
			if (EOF == ch)
				break;
			buffer.push_back(ch);
		}

		fclose(file);
		return buffer;
	}
	else
		return "";
}

bool ShaderProgram::AddAndCompile(const std::string& shaderPath, EShaderType type)
{
	std::string shaderCode = Load(shaderPath);
	if ("" == shaderCode)
	{
		m_ErrorInfo = "\n Error : couldn't load file from path: " + shaderPath;
		printf(m_ErrorInfo.c_str());
		return false;
	}

	GLenum shaderType;
	switch (type)
	{
	case EShaderType::VERTEX:
		shaderType = GL_VERTEX_SHADER;
		break;
	case EShaderType::FRAGMENT:
		shaderType = GL_FRAGMENT_SHADER;
		break;
	default:

		break;
	}

	const char* code = shaderCode.c_str();
	GLuint      shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &code, NULL);
	glCompileShader(shaderID);

	if (ErrorOccured(shaderID, false))
	{
		glDeleteShader(shaderID);
		return false;
	}
	m_ShaderIDs.push_back(shaderID);
	return true;
}

bool ShaderProgram::Build()
{
	m_ID = glCreateProgram();
	for (GLuint shaderID : m_ShaderIDs)
		glAttachShader(m_ID, shaderID);

	glLinkProgram(m_ID);

	if (ErrorOccured(m_ID, true))
	{
		glDeleteShader(m_ID);
		return false;
	}

	for (GLuint shaderID : m_ShaderIDs)
		glDeleteShader(shaderID);

	m_IsReadyToUse = true;

	return true;
}

bool ShaderProgram::ErrorOccured(GLuint id, bool linking)
{
	GLint success;

	if (linking)
	{
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLchar errInfo[512];
			GLsizei charWritten;
			glGetProgramInfoLog(id, 512, &charWritten, errInfo);
			m_ErrorInfo = "Linking " + m_Name + " :: Error :: " + std::string(errInfo);
			printf("%s\n", m_ErrorInfo.c_str());
			return true;
		}
	}
	else
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar errInfo[512];
			GLsizei charWritten;
			glGetShaderInfoLog(id, 512, &charWritten, errInfo);
			m_ErrorInfo = "Compilation " + m_Name + " :: Error :: " + std::string(errInfo);
			printf("%s\n", m_ErrorInfo.c_str());
			return true;
		}
	}

	return false;
}

void ShaderProgram::Use()
{
	if (m_IsReadyToUse)
		glUseProgram(m_ID);
}

void ShaderProgram::SetInt(const std::string& pName, int pValue)
{
	glUniform1i(glGetUniformLocation(m_ID, pName.c_str()), pValue);
}

void ShaderProgram::SetFloat(const std::string& pName, float pValue)
{
	glUniform1f(glGetUniformLocation(m_ID, pName.c_str()), pValue);
}

void ShaderProgram::SetVec2(const std::string& pName, const glm::vec2& pVector)
{
	glUniform2f(glGetUniformLocation(m_ID, pName.c_str()), pVector.x, pVector.y);
}

void ShaderProgram::SetVec3(const std::string& pName, const glm::vec3& pVector)
{
	glUniform3f(glGetUniformLocation(m_ID, pName.c_str()), pVector.x, pVector.y, pVector.z);
}

void ShaderProgram::SetVec4(const std::string& pName, const glm::vec4& pVector)
{
	glUniform4f(glGetUniformLocation(m_ID, pName.c_str()), pVector.x, pVector.y, pVector.z, pVector.w);
}

void ShaderProgram::SetMat4(const std::string& pName, const glm::mat4& pMat)
{
	int id = glGetUniformLocation(m_ID, pName.c_str());
	glUniformMatrix4fv(glGetUniformLocation(m_ID, pName.c_str()), 1, GL_FALSE, glm::value_ptr(pMat));
}

void ShaderProgram::SetNMat4(const std::string& pName, std::vector<glm::mat4> transforms, int size)
{
	int id = glGetUniformLocation(m_ID, pName.c_str());
	glUniformMatrix4fv(id, size, GL_FALSE, (GLfloat*)&transforms[0]);
}
