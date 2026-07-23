#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "include/stb_image/stb_image.h"
#include "Camera.h"
// #include "Texture.h"
// #include "Mesh.h"
#include "Model.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
using namespace std;
	

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 70.0f;
Camera camera(fov, 2.5f, 50.0f, 800.0f/600.0f);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspectRatio = ((float) width) / height;
}

void processInput(GLFWwindow* window) {
	camera.processInput(window, deltaTime);

	// if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	// 	glfwSetWindowShouldClose(window, true);

	// if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
	// 	camera.moveForward(deltaTime);
	// }
	// if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
	// 	camera.moveBackward(deltaTime);
	// }
	// if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
	// 	camera.moveLeft(deltaTime);
	// }
	// if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
	// 	camera.moveRight(deltaTime);
	// }
	// if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
	// 	camera.moveUp(deltaTime);
	// }
	// if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
	// 	camera.moveDown(deltaTime);
	// }
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
	camera.updateCameraRotation(xPos, yPos, deltaTime);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.onScroll(yOffset);
}

int main(void) {
	cout << "starting main" << endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Da Engine", nullptr, nullptr);
	if (window == NULL) {
		cout << "Failed to create window." << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	Shader shaders("shaders/vertex.vs", "shaders/fragment.fs");
	// Shader lightShader("shaders/vertex.vs", "shaders/lightFragment.fs");


	

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// unsigned int indices[] = {
	// 	0, 1, 3,
	// 	1, 2, 3
	// };

	// unsigned int VAO, VBO;//, EBO;
	unsigned int VBO;
	// glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// glBindVertexArray(VAO);

	// // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	// glEnableVertexAttribArray(0);
	// glEnableVertexAttribArray(1);
	// glEnableVertexAttribArray(2);

	
	// glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind vbo
	// glBindVertexArray(0); //unbind vao -----------------------------------------------------------

	unsigned int lightVAO; //-------------------------------- light vao
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind the cube vbo so the data is the sameig
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // ------------------------------------------

	Model backpack("backpack/backpack.obj");


	// Texture texture1("textures/container.jpg", GL_TEXTURE0);
	// Texture texture2("textures/awesomeface.png", GL_TEXTURE1);

	shaders.use();
	
	// Texture diffuseMap("textures/container2.png", GL_TEXTURE0);
	// Texture specularMap("textures/container2_specular.png", GL_TEXTURE1);
	// shaders.setInt("material.diffuse", 0);
	// shaders.setInt("material.specular", 1);
	// shaders.setFloat("material.shininess", 32.0f);

	shaders.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shaders.setVec3("dirLight.ambient",  0.2f, 0.2f, 0.2f);
	shaders.setVec3("dirLight.diffuse",  0.5f, 0.5f, 0.5f);
	shaders.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};
	for (int i = 0; i < 4; i++) {
		shaders.setFloat("pointLights[" + std::to_string(i) + "].constant",  1.0f);
		shaders.setFloat("pointLights[" + std::to_string(i) + "].linear",    0.09f);
		shaders.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
		shaders.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);	
	}
	
	shaders.setVec3("spotlight.position",  camera.eye);
	shaders.setVec3("spotlight.direction", camera.dir);
	shaders.setFloat("spotlight.innerCutoff", glm::cos(glm::radians(12.5f)));
	shaders.setFloat("spotlight.outerCutoff", glm::cos(glm::radians(17.5f)));
	shaders.setFloat("spotlight.constant",  1.0f);
	shaders.setFloat("spotlight.linear",    0.09f);
	shaders.setFloat("spotlight.quadratic", 0.032f);
	shaders.setVec3("spotlight.ambient", 0.1f, 0.1f, 0.1f);
	shaders.setVec3("spotlight.diffuse", 0.8f, 0.8f, 0.8f);
	shaders.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);
	
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		processInput(window);
		
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
		shaders.use();
		shaders.setMat4("model", model);
		shaders.setMat4("view", camera.getViewMatrix());
		shaders.setMat4("projection", camera.getProjectionMatrix());
		shaders.setMat3("normalMatrix", normalMatrix);
		// shaders.setVec3("camPos", camera.eye);
		// shaders.setVec3("spotlight.position",  camera.eye);
		// shaders.setVec3("spotlight.direction", camera.dir);
		// diffuseMap.bindTexture();
		// specularMap.bindTexture();
		// glBindVertexArray(VAO);
		
		backpack.render(shaders);
		
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// lightShader.use();
		// lightShader.setMat4("view", camera.getViewMatrix());
		// lightShader.setMat4("projection", camera.getProjectionMatrix());
		// glBindVertexArray(lightVAO);
		

		// glm::mat4 lightPosModel = glm::mat4(1.0f);
		// lightPosModel = glm::translate(lightPosModel, lightPos);
		// lightPosModel = glm::scale(lightPosModel, glm::vec3(0.2f));
		// lightShader.setMat4("model", lightPosModel);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// for (const auto& p: pointLightPositions) {
		// 	glm::mat4 lightPosModel = glm::mat4(1.0f);
		// 	lightPosModel = glm::translate(lightPosModel, p);
		// 	lightPosModel = glm::scale(lightPosModel, glm::vec3(0.2f));
		// 	lightShader.setMat4("model", lightPosModel);
		// 	glDrawArrays(GL_TRIANGLES, 0, 36);
		// }



		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}