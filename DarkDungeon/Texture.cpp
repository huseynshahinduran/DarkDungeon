#include "Texture.h"
#include "STB.H" //Library used for loading image files
#include <iostream>

Texture::Texture()
{
	textureHandler = 0; //Initialize with 0 to indicate no texture is loaded yet
}

Texture::~Texture()
{
	//Clean up OpenGL texture memory from the GPU when the object is destroyed
	if (textureHandler != 0)
		glDeleteTextures(1, &textureHandler);
}

void Texture::init(const std::string& fileName)
{
	int width, height, numComponents;

	//Load image data from disk, forcing 4 components for RGBA (Red, Green, Blue, Alpha)
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numComponents, 4);

	//Check if the image failed to load
	if (imageData == NULL)
	{
		std::cerr << "Texture loading failed: " << fileName << std::endl;
		return;
	}

	//Generate a new OpenGL texture ID and bind it so we can configure it
	glGenTextures(1, &textureHandler);
	glBindTexture(GL_TEXTURE_2D, textureHandler);

	//Set texture wrapping behavior to repeat the image if coordinates go beyond 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Set texture filtering to linear for smooth pixel blending when scaling
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Upload the raw pixel data to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	
	//Automatically generate smaller versions of the texture for distant rendering
	glGenerateMipmap(GL_TEXTURE_2D);

	//Free the CPU memory since the data now lives on the GPU
	stbi_image_free(imageData);

	std::cout << "Texture loaded: " << fileName << " (" << width << "x" << height << ")" << std::endl;
}

void Texture::Bind(unsigned int unit)
{
	//Activate the specified texture slot (0-31) and bind our texture to it
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureHandler);
}