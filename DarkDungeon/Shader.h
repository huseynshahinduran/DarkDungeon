#pragma once

#include <string>
#include <GL/glew.h>
#include "transform.h"
#include "Camera.h"

//Manages the loading, compiling, and linking of OpenGL shader programs
class Shader
{
public:
	Shader();
	~Shader();

	//Initializes the shader program using vertex and fragment shader files
	void init(const std::string& vertFile, const std::string& fragFile);
	void Bind(); //Activities the shader program for use in rendering

	//Updates shader uniforms (like transformation matrices) per frame
	void Update(const Transform& transform, const Camera& camera);

	//Helpers to send data (uniforms) directly to the shader program
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, float x, float y, float z);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setMat4(const std::string& name, const glm::mat4& value);

	//Retrurns the OpenGL handle for the compiled shader program
	GLuint getProgram() const { return program; }

private:
	static const unsigned int NUM_SHADERS = 2; //Usually Vertex and Fragment

	//reads shader source code from a text file
	std::string LoadShader(const std::string& fileName);

	//Utility to check and log compilation/linking errors
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	
	//Compiles an individual shader stage
	GLuint CreateShader(const std::string& text, unsigned int type);

	GLuint program; //The final linked shader program ID
	GLuint shaders[NUM_SHADERS]; //IDs for the individual shaders 

	//Tracks locations of uniform variables
	enum
	{
		TRANSFORM_U,
		NUM_UNIFORMS
	};

	GLuint uniforms[NUM_UNIFORMS];
};