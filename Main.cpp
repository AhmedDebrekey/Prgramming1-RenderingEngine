#include <iostream>
#include <unordered_map> //TODO: Loading models in render loop

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "stb_image.h"

#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Model.h"

//Window Props ... Must be the same in the PerspectiveCamera Class as well
static constexpr uint32_t WindowHeight = 1200;
static constexpr uint32_t WindowWidth = 900;

int main(void)
{
	//Initialization of GLFW
	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Glfw window declaration
	GLFWwindow* m_Window = glfwCreateWindow(mode->width, mode->height, "RenderingEngine", monitor, NULL);
	if (!m_Window)
	{
		std::cout << "[ERROR] Failed to create a Window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_Window);

	//Initialization of GLAD
	gladLoadGL();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Window Viewport
	glViewport(0, 0, mode->width, mode->height);

	//Flip Textures on the x axis (vertically)
	//stbi_set_flip_vertically_on_load(true);

	//For the "light" cube
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	//Modeling Loading
	Model ourModel("Resources/Sponza/scene.gltf");

	//Shader class declaration 
	Shader ourShader;
	ourShader.Create("VertexShader.glsl", "FragmentShader.glsl"); //Using the Vertex and Fragment shaders
	ourShader.Bind();
	ourShader.Unbind();

	Shader PointLightShader;
	PointLightShader.Create("LightVertex.glsl", "LightFragment.glsl");
	PointLightShader.Bind();
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Unbinding everything
	ourShader.Unbind();
	PointLightShader.Unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Enabling depth testing (z-buffer)
	glEnable(GL_DEPTH_TEST);

	//ImGUI boilerplate code
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	

	//Perspective camera declaration
	Camera camera(m_Window);
	//5.0 , -0.1 , -1.0

	//Varaiables for ImGUI
	glm::vec3 ModelPos = glm::vec3(5.0f, -0.1f, -1.0f);
	glm::vec1 ModelScale = glm::vec3(0.01f);
	glm::vec3 MapColor = glm::vec3(0.369f, 0.259f, 0.259f);
	glm::vec3 pointLightColors = glm::vec3(1.0f, 0.6f, 0.0f);

	glm::vec3 PointLightPos = glm::vec3(0.0f);

	//Main loop
	/* Loop until the user closes the window */
	while (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(m_Window))
	{
		//Clearing the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(MapColor[0], MapColor[1], MapColor[2], 1.0);

		//Also ImGUI boilerplate code
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Computing the camera matricies
		camera.Compute();

		ourShader.Bind();

		ourShader.SetVec3("viewPos", camera.Position());

		ourShader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		ourShader.SetVec3("dirLight.ambient", glm::vec3(MapColor));
		ourShader.SetVec3("dirLight.diffuse", glm::vec3(MapColor));
		ourShader.SetVec3("dirLight.specular", glm::vec3(MapColor));

		ourShader.SetVec3("pointLights.position", glm::vec3(0.0f,0.0f,0.0f)); //change the position, and render a lamp please
		ourShader.SetVec3("pointLights.ambient", glm::vec3(pointLightColors.x * 0.1, pointLightColors.y * 0.1, pointLightColors.z * 0.1));
		ourShader.SetVec3("pointLights.diffuse", pointLightColors);
		ourShader.SetVec3("pointLights.specular", pointLightColors);
		ourShader.SetFloat("pointLights.constant", 1.0f);
		ourShader.SetFloat("pointLights.linear", 0.09);
		ourShader.SetFloat("pointLights.quadratic", 0.032);

		glm::mat4 projection = camera.ProjectionMatrix();
		glm::mat4 view = camera.ViewMatrix();
		ourShader.SetMat4f("projection", projection);
		ourShader.SetMat4f("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, ModelPos); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(ModelScale, ModelScale, ModelScale));
		//model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0, 1, 0));
		ourShader.SetMat4f("model", model);
		ourModel.Draw(ourShader);
		ourShader.Unbind();

		//Model loading Stops here

		PointLightShader.Bind();
		glBindVertexArray(VAO);
		PointLightShader.SetMat4f("projection", camera.ProjectionMatrix());
		PointLightShader.SetMat4f("view", camera.ViewMatrix());
		glm::mat4 pointmodel = glm::mat4(1.0f);
		PointLightShader.SetMat4f("model", pointmodel);
		PointLightShader.SetVec3("Color", glm::vec3(pointLightColors));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		PointLightShader.Unbind();

		//ImGUI window
		ImGui::Begin("Settings");
		ImGui::Text("Welcome to Debreky's Rendering Engine");
		ImGui::SliderFloat3("Model Postion", &ModelPos[0], -10.0F, 10.0F);
		ImGui::SliderFloat("Model Scale", &ModelScale[0],  0.01F, 1.0F);
		ImGui::ColorEdit3("Map Color", &MapColor[0]);
		ImGui::ColorEdit3("Point Light Color", &pointLightColors[0]);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(m_Window);

		// Poll for and process events
		glfwPollEvents();
	}

	//Terminating everything 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}