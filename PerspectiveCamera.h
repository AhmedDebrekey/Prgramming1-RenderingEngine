#pragma once
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(GLFWwindow* window);
	~Camera();

	void Compute();

	glm::mat4 ProjectionMatrix();
	glm::mat4 ViewMatrix();
	glm::vec3 Position();
	glm::vec3 Front();

private:
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
	GLFWwindow* m_Window;

	glm::vec3 m_Position = glm::vec3(0, 0, -3);
	glm::vec3 m_Direction = glm::vec3(0);
	// Initial horizontal angle : toward -Z
	float m_HorizontalAngle = 1.0f;
	// Initial vertical angle : none
	float m_VerticalAngle = 0.0f;
	// Initial Field of View
	float m_InitialFoV = 80.0f;

	float m_Speed = 5.0f; // 3 units / second
	float m_MouseSpeed = 0.003f;

};
