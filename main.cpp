#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "include/stb_image/stb_image.h"
#include "Camera.h"
#include "FirstPersonControls.h"
#include "OrbitControls.h"
// #include "Texture.h"
// #include "Mesh.h"
#include "Model.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>
#include "SceneObject.h"
#include "Material.h"
using namespace std;
	

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 70.0f;
const float WINDOW_WIDTH = 1280.0f;
const float WINDOW_HEIGHT = 720.0f;

// FirstPersonControls camera(fov, 2.5f, 50.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
OrbitControls camera(fov, 2.5f, 50.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspectRatio = ((float) width) / height;
}

void processInput(GLFWwindow* window, bool shouldCaptureMouse) {
	camera.processInput(window, deltaTime, shouldCaptureMouse);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
	camera.updateCameraRotation(window, xPos, yPos, deltaTime);
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

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Da Engine", nullptr, nullptr);
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

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
	ImGuiStyle& style = ImGui::GetStyle();
	style.FontScaleDpi = main_scale * 1.25f;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	Shader diffuseShader("shaders/vertex.vs", "shaders/fragment.fs"); // diffuse
	Shader colorShader("shaders/basic/basic.vs", "shaders/basic/basic.fs"); // solid color only


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

	// unsigned int lightVAO; //-------------------------------- light vao
	// glGenVertexArrays(1, &lightVAO);
	// glBindVertexArray(lightVAO);

	// glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind the cube vbo so the data is the sameig
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	// glEnableVertexAttribArray(0);

	// glBindVertexArray(0); // ------------------------------------------


	// Texture texture1("textures/container.jpg", GL_TEXTURE0);
	// Texture texture2("textures/awesomeface.png", GL_TEXTURE1);

	// shaders.use();
	
	// Texture diffuseMap("textures/container2.png", GL_TEXTURE0);
	// Texture specularMap("textures/container2_specular.png", GL_TEXTURE1);
	// shaders.setInt("material.diffuse", 0);
	// shaders.setInt("material.specular", 1);
	// shaders.setFloat("material.shininess", 32.0f);

	// shaders.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	// shaders.setVec3("dirLight.ambient",  0.2f, 0.2f, 0.2f);
	// shaders.setVec3("dirLight.diffuse",  0.5f, 0.5f, 0.5f);
	// shaders.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};
	// for (int i = 0; i < 4; i++) {
	// 	shaders.setFloat("pointLights[" + std::to_string(i) + "].constant",  1.0f);
	// 	shaders.setFloat("pointLights[" + std::to_string(i) + "].linear",    0.09f);
	// 	shaders.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	// 	shaders.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);	
	// }
	
	// shaders.setVec3("spotlight.position",  camera.eye);
	// shaders.setVec3("spotlight.direction", camera.dir);
	// shaders.setFloat("spotlight.innerCutoff", glm::cos(glm::radians(12.5f)));
	// shaders.setFloat("spotlight.outerCutoff", glm::cos(glm::radians(17.5f)));
	// shaders.setFloat("spotlight.constant",  1.0f);
	// shaders.setFloat("spotlight.linear",    0.09f);
	// shaders.setFloat("spotlight.quadratic", 0.032f);
	// shaders.setVec3("spotlight.ambient", 0.1f, 0.1f, 0.1f);
	// shaders.setVec3("spotlight.diffuse", 0.8f, 0.8f, 0.8f);
	// shaders.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);
	
	bool isSceneOpen = true;
	std::vector<SceneObject> sceneObjects;


	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		cout << "fps: " << 1.0f / deltaTime << "\r";

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Scene", &isSceneOpen);
		ImGui::Text("Objects");
		if (ImGui::Button("Add Primitive")) {
			SceneObject sphere("models/sphere.obj");
			sphere.material = std::make_unique<ColorMaterial>(colorShader, glm::vec3(0.0f, 0.0f, 1.0f));
			sphere.name = "Sphere " + std::to_string(sceneObjects.size() + 1);

			sceneObjects.push_back(std::move(sphere));
		}
		if (ImGui::Button("Add Backpack")) {
			SceneObject bag("models/backpack/backpack.obj");
			bag.material = std::make_unique<DiffuseMaterial>(diffuseShader);
			bag.name = "Backpack " + std::to_string(sceneObjects.size() + 1);

			sceneObjects.push_back(std::move(bag));
		}

		static int index = 0;
		std::vector<const char*> labels;
		for (auto& obj: sceneObjects) {
			labels.push_back(obj.name.c_str());
		}
		ImGui::Text("Scene");
		ImGui::ListBox("##SceneListBox", &index, labels.data(), labels.size());
		ImGui::End();
		

		if (sceneObjects.size() > 0) {
			ImGui::Begin("Transform");
			ImGui::Text(sceneObjects[index].name.c_str());
			ImGui::DragFloat3("Position", glm::value_ptr(sceneObjects[index].position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(sceneObjects[index].rotation), 0.1f);
			ImGui::DragFloat3("Scale",    glm::value_ptr(sceneObjects[index].scale),    0.1f);
			
			ImGui::End();
		}

		ImGui::EndFrame();

		

		processInput(window, !io.WantCaptureMouse);
		
		// glm::mat4 model = glm::mat4(1.0f);
		// glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
		// basic.use();
		// basic.setMat4("model", model);
		// basic.setMat4("view", camera.getViewMatrix());
		// basic.setMat4("projection", camera.getProjectionMatrix());
		// basic.setMat3("normalMatrix", normalMatrix);
		// basic.setVec3("color", 0.0f, 0.0f, 1.0f);


		// shaders.setVec3("camPos", camera.eye);
		// shaders.setVec3("spotlight.position",  camera.eye);
		// shaders.setVec3("spotlight.direction", camera.dir);
		// diffuseMap.bindTexture();
		// specularMap.bindTexture();
		// glBindVertexArray(VAO);
		
		// backpack.render(shaders);
		for (SceneObject& m: sceneObjects) {
			// do m.shader.use
			m.material->shader.use();
			// set the object model matrix
			m.material->shader.setMat4("model", m.getModelMatrix());
			// set the view and projection matrices
			m.material->shader.setMat4("view", camera.getViewMatrix());
			m.material->shader.setMat4("projection", camera.getProjectionMatrix());	
			// call the object.material.applyuniforms
			m.material->apply();
			// call object render
			m.model.render(m.material->shader);
		}

		
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