#include "Shader.h"
#include <iostream>
#include <fstream>

Shader::Shader()
{
	program = 0; //Initialize with 0 to indicate no program has been created yet
}

Shader::~Shader()
{
	//Clean up OpenGL memory if the program exists
	if (program == 0) return;

	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		//Disconnect and delete the individual compiled shaders
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	//Delete the final linked program
	glDeleteProgram(program);
}

void Shader::init(const std::string& vertFile, const std::string& fragFile)
{
	//Create an empty OpenGL shader program
	program = glCreateProgram();

	//Load and compile the vertex and fragment shaders
	shaders[0] = CreateShader(LoadShader(vertFile), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(fragFile), GL_FRAGMENT_SHADER);

	//Attach the compiled shaders to the main program
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(program, shaders[i]);

	//Tell OpenGL which shader variables correspond to our Vertex struct data
	glBindAttribLocation(program, 0, "VertexPosition");
	glBindAttribLocation(program, 1, "TexCoords");
	glBindAttribLocation(program, 2, "VertexNormal");

	//Link the shaders together into the final executable program
	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: Shader program linking failed");

	//Verify the program can execute in the current OpenGL state
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: Shader program not valid");

	//Find and store the location of the "transform" (MVP matrix) uniform variable
	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform");
}

void Shader::Bind()
{
	//Set this shader program as the active one for rendering
	glUseProgram(program);
}

void Shader::Update(const Transform& transform, const Camera& camera)
{
	//Calculate the Model-View-Projection (MVP) matrix
	glm::mat4 mvp = camera.GetViewProjection() * transform.GetModel();

	//Send the calculated MVP matrix to the shader
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, &mvp[0][0]);
}

//--- Helper functions to send custom data to the shader ---
void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

//--- Internal utility functions ---
std::string Shader::LoadShader(const std::string& fileName)
{
	//Open the shader text file
	std::ifstream file;
	file.open(fileName.c_str());

	std::string output;
	std::string line;

	//Read the file line by line into a single string
	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	//Check the status (compilation for shaders, linking/validation for programs)
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	//If it failed, retrieve and print the error log
	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
	//Generate an empty shader of the specified type (Vertex or Fragment)
	GLuint shader = glCreateShader(type);

	if (shader == 0)
		std::cerr << "Error: Shader creation failed for type " << type << std::endl;

	//Pass the source code string to OpenGL
	const GLchar* stringSource[1];
	stringSource[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, stringSource, lengths);

	//Compile the shader code
	glCompileShader(shader);
	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

	return shader;
}