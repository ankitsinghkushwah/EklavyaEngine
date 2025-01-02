// Declaration header

#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <SOIL.h>

using namespace Eklavya::Asset;

Texture2D::Texture2D() : mImage(nullptr)
{
}

Texture2D::Texture2D(const std::string& assetName) : IAsset(EType::TEXTURE, assetName)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mID);
	SOIL_free_image_data(mImage);
}

int Texture2D::CreateTexture(std::string path, bool repeat)
{
	int width, height;
	int channels;

	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, 0);
	if (image == nullptr)
	{
		printf("\n failed to load image from path %s, SOIL reason : ", path.c_str(), result_string_pointer);
		return -1;
	}
	mImage = image;
	mSize = glm::vec2(width, height);

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	GLenum format;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;

	mFormat = format;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, mImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return mID;
}

int Texture2D::CreateTextureFromBuffer(unsigned int bufferSize, void* imageData)
{
	int width, height;
	int channels;

	unsigned char* image = SOIL_load_image_from_memory((const unsigned char* const)imageData, bufferSize, &width, &height, &channels, 0);
	if (image == nullptr)
	{
		printf("\n failed to load image from buffer");
		return -1;
	}
	mImage = image;
	mSize = glm::vec2(width, height);

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	GLenum format;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;

	mFormat = format;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, mImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return mID;
}

void Texture2D::BindToUnit(uint32_t textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, mID);
}

void Texture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
