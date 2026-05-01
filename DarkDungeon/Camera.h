#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//Represents a 3D camera to navigate and view the scene
struct Camera
{
	glm::mat4 projection; //Defines the field of view and clipping distances
	glm::vec3 pos; //Camera's location in 3D space
	glm::vec3 forward; //The direction the camera is looking
	glm::vec3 up; //Defines which direction is "up"

	float yaw; //Horizontal rotation (looking left/right)
	float pitch; //Vertical rotation (looking up/down)

	//Initializes default camera values
	Camera()
	{
		projection = glm::mat4(1.0f);
		pos = glm::vec3(0.0f);
		forward = glm::vec3(0.0f, 0.0f, 1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = 90.0f;
		pitch = 0.0f;
	}

	//Sets up the camera's starting position and perspective lens
	void initCamera(const glm::vec3& p, float fov, float aspect, float nearClip, float farClip)
	{
		pos = p;
		forward = glm::vec3(0.0f, 0.0f, 1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = 90.0f;
		pitch = 0.0f;
		projection = glm::perspective(fov, aspect, nearClip, farClip);
	}

	//Updates the camera's viewing direction based on mouse input
	void rotate(float xOffset, float yOffset)
	{
		yaw += xOffset;
		pitch += yOffset;

		//Constrain the pitch so the screen doesn't flip upside down
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		//Calculate the new forward direction vector using trigonometry
		glm::vec3 newForward;
		newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newForward.y = sin(glm::radians(pitch));
		newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(newForward);
	}

	//Calculates the combined View-Projection matrix for rendering
	glm::mat4 GetViewProjection() const
	{
		//glm::lookAt creates the View matrix based on position and target
		return projection * glm::lookAt(pos, pos + forward, up);
	}

	//Returns the camera's current position
	glm::vec3 getPos() const
	{
		return pos;
	}
};