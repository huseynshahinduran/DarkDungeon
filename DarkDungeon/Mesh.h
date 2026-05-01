#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "obj_loader.h"

//Represents a single point of a 3D model with spatial and texture data
struct Vertex
{
public:
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord)
	{
		this->pos = pos;
		this->texCoord = texCoord;
		this->normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec3* GetPos() { return &pos; }
	glm::vec2* GetTexCoord() { return &texCoord; }
	glm::vec3* GetNormal() { return &normal; }

private:
	glm::vec3 pos; //3D position coordinates
	glm::vec2 texCoord; //2D texture mapping coordinates
	glm::vec3 normal; //Direction the vertex is facing (used for lighting)
};

//Manages the OpenGL buffers required to render a 3D object
class Mesh
{
public:
	Mesh();
	~Mesh();

	//Renders the mesh to the screen
	void draw();

	//Initializes the mesh using raw vertex and index arrays
	void init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	
	//Loads and initializes a mesh from an external 3D model file (e.g., .obj)
	void loadModel(const std::string& filename);

	//Initializes the mesh using a pre-parsed model structure
	void initModel(const IndexedModel& model);

private:
	//Indetifiers for the different types of data buffers
	enum
	{
		POSITION_VERTEXBUFFER,
		TEXCOORD_VB,
		NORMAL_VB,
		INDEX_VB,
		NUM_BUFFERS
	};

	GLuint vertexArrayObject; //OpenGL ID for the Vertex Array Object (VAO)
	GLuint vertexArrayBuffers[NUM_BUFFERS]; //OpenGL IDs for the Vertex Buffer Objects (VBOs)
	unsigned int drawCount; //Total number of elements/indices to draw
};