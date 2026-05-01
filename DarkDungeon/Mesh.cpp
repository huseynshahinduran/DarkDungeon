#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
{
	drawCount = 0;
	vertexArrayObject = 0; //0 indicates the VAO hasn't been generated yet
	for (int i = 0; i < NUM_BUFFERS; i++)
		vertexArrayBuffers[i] = 0;
}

Mesh::~Mesh()
{
	//Clean up the Vertex Array Object from GPU memory when the mesh is destroyed
	if (vertexArrayObject != 0)
		glDeleteVertexArrays(1, &vertexArrayObject);
}

void Mesh::loadModel(const std::string& filename)
{
	//Parse the OBJ file and convert it to a structured format with indices
	IndexedModel model = OBJModel(filename).ToIndexedModel();

	//Check if the file failed to load or contained no geometry
	if (model.positions.size() == 0 || model.indices.size() == 0)
	{
		std::cerr << "Warning: Model empty or failed to load: " << filename << std::endl;
		return;
	}

	std::cout << "Model loaded: " << filename
		<< " (" << model.positions.size() << " vertices, "
		<< model.indices.size() << " indices)" << std::endl;

	//Send the loaded data to the GPU
	initModel(model);
}

void Mesh::init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	IndexedModel model;

	//Manually convert the raw arrays of vertices into the IndexedModel format
	for (unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
		model.normals.push_back(*vertices[i].GetNormal());
	}

	//Copy the index data
	for (unsigned int i = 0; i < numIndices; i++)
		model.indices.push_back(indices[i]);

	//Send the structured data to the GPU
	initModel(model);
}

void Mesh::initModel(const IndexedModel& model)
{
	//Store how many vertices we actually need to draw
	drawCount = model.indices.size();

	//Generate and bind the Vertex Array Object (VAO) to store our buffer configurations
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//Generate the Vertex Buffer Objects (VBOs) for our different data types
	glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

	//1. Setup Position data (Attribute 0)
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VERTEXBUFFER]);
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//2. Setup Texture Coordinate data (Attribute 1)
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//3. Setup Normal vector data (Attribute 2)
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//4. Setup Index data (tells OpenGL what order to connect the vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

	///Unbind the VAO to prevent accidental modifications later
	glBindVertexArray(0);
}

void Mesh::draw()
{
	//Skip drawing if there's no geometry
	if (drawCount == 0)
		return;

	//Bind the VAO containing all our buffer state
	glBindVertexArray(vertexArrayObject);

	//Command the GPU to draw the elements using our index buffer
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);

	//Unbind the VAO
	glBindVertexArray(0);
}