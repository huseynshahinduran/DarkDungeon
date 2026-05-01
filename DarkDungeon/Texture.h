#pragma once

#include <string>
#include <GL/glew.h>

//Manages the loading and binding of an OpenGL texture
class Texture
{
public:
	Texture();
	~Texture();

	//Loads a texture from the specified file path
	void init(const std::string& fileName);

	//Binds the texture to a specific texture unit for rendering
	void Bind(unsigned int unit);

	//Returns the OpenGL texture handle/ID
	GLuint getID() const { return textureHandler; }

private:
	//OpenGL ID associated with this texture
	GLuint textureHandler;
};