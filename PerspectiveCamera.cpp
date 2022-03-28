#include "PerspectiveCamera.h"

#include <iostream>


Camera::Camera(GLFWwindow* window)
	: m_ProjectionMatrix(glm::mat4(1.0f)), m_ViewMatrix(glm::mat4(1.0f)), m_Window(window)
{
	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);
	glfwSetCursorPos(m_Window, (double)width / 2, (double)height / 2);
}

Camera::~Camera()
{
}

void Camera::Compute() {

	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos;
	glfwGetCursorPos(m_Window, &xpos, &ypos);


	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	//Only compute mouse movement if space key is pressed
	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_MouseSpeed = 0.0f;
		glfwSetCursorPos(m_Window, double(mode->width / 2), double(mode->height / 2));
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //hide cursor
		m_MouseSpeed = 0.003f;
		m_HorizontalAngle += m_MouseSpeed * float(mode->width / 2 - xpos);// change these numbers here if you wanna change screen width and height
		m_VerticalAngle += m_MouseSpeed * float(mode->height / 2 - ypos);//smae for this number
	}
	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //show cursor
	}


	//Complicated math to compute the camera stuff and thingies lol 
	m_Direction = {
		cos(m_VerticalAngle) * sin(m_HorizontalAngle),
		sin(m_VerticalAngle),
		cos(m_VerticalAngle) * cos(m_HorizontalAngle)
	};

	glm::vec3 right = glm::vec3(
		sin(m_HorizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_HorizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 upwards = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 up = glm::cross(right, m_Direction);

	//Movement controlls
	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) { //forward
		m_Position += m_Direction * deltaTime * m_Speed;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) { //backwards
		m_Position -= m_Direction * deltaTime * m_Speed;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) { //right
		m_Position += right * deltaTime * m_Speed;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {//left
		m_Position -= right * deltaTime * m_Speed;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS) {//up
		m_Position += upwards * deltaTime * m_Speed;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS) {//down
		m_Position -= upwards * deltaTime * m_Speed;
	}

	float FoV = m_InitialFoV;
	m_ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	m_ViewMatrix = glm::lookAt( //thank god for glm lookAt function, for doing it's work otherwise it would be pretty complicated and yea :)
		m_Position,           
		m_Position + m_Direction,
		up                  
	);

	lastTime = currentTime;
}

glm::mat4 Camera::ProjectionMatrix()
{
	return m_ProjectionMatrix;
}

glm::mat4 Camera::ViewMatrix()
{
	return m_ViewMatrix;
}

glm::vec3 Camera::Position()
{
	return m_Position;
}

glm::vec3 Camera::Front()
{
	return m_Direction;
}
