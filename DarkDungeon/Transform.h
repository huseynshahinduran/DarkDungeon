#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//Represents a 3D transformation (position, rotation, scale)
struct Transform
{
	Transform(const glm::vec3& pos = glm::vec3(),
		const glm::vec3& rot = glm::vec3(),
		const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->pos = pos;
		this->rot = rot;
		this->scale = scale;
	}

	//Calculates and returns the 4x4 Model matrix
	glm::mat4 GetModel() const
	{
		glm::mat4 posMatrix = glm::translate(pos);
		glm::mat4 rotXMatrix = glm::rotate(rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotYMatrix = glm::rotate(rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotZMatrix = glm::rotate(rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scaleMatrix = glm::scale(scale);

		//Combine rotations (Z * Y * X)
		glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

		//Apply transformations in order: Scale, then Rotate, then Translate
		return posMatrix * rotMatrix * scaleMatrix;
	}

	glm::vec3& GetPos() { return pos; }
	glm::vec3& GetRot() { return rot; }
	glm::vec3& GetScale() { return scale; }

	void SetPos(const glm::vec3& pos) { this->pos = pos; }
	void SetRot(const glm::vec3& rot) { this->rot = rot; }
	void SetScale(const glm::vec3& scale) { this->scale = scale; }

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
};