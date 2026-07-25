#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include "Shader.h"
#include "include/stb_image/stb_image.h"
#include "Camera.h"
#include "FirstPersonControls.h"
#include "OrbitControls.h"
#include "Model.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <vector>
#include "SceneObject.h"
#include "Material.h"
#include <unordered_map>
using namespace std;
	

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 70.0f;
const float WINDOW_WIDTH = 1280.0f;
const float WINDOW_HEIGHT = 720.0f;

const int MAX_POINT_LIGHTS = 10;

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
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) return;

	camera.onScroll(yOffset);
}

int main(void) {
	cout << "starting main" << endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad ;//| ImGuiConfigFlags_DockingEnable;
	io.IniFilename = nullptr;
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
	ImGuiStyle& style = ImGui::GetStyle();
	style.FontScaleDpi = main_scale * 1.25f;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// call em once so that the constructor runs and everything else can cccess these globally
	getColorShader();
	getDiffuseShader();
	getLitShader();

	// Shader lightShader("shaders/vertex.vs", "shaders/lightFragment.fs");

	// shaders.setInt("material.diffuse", 0);
	// shaders.setInt("material.specular", 1);
	// shaders.setFloat("material.shininess", 32.0f);

	// shaders.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	// shaders.setVec3("dirLight.ambient",  0.2f, 0.2f, 0.2f);
	// shaders.setVec3("dirLight.diffuse",  0.5f, 0.5f, 0.5f);
	// shaders.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

	// glm::vec3 pointLightPositions[] = {
	// 	glm::vec3( 0.7f,  0.2f,  2.0f),
	// 	glm::vec3( 2.3f, -3.3f, -4.0f),
	// 	glm::vec3(-4.0f,  2.0f, -12.0f),
	// 	glm::vec3( 0.0f,  0.0f, -3.0f)
	// };
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
	std::unordered_map<unsigned int, std::vector<SceneObject*>> shaderGroups;
	vector<PointLight> pointLights;

	unsigned int pointLightUBO;
	glGenBuffers(1, &pointLightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightUBO);
	glBufferData(GL_UNIFORM_BUFFER, (sizeof(PointLightGPUStruct) * MAX_POINT_LIGHTS) + 4, nullptr, GL_DYNAMIC_DRAW); // +4 is the numlights uniform
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, pointLightUBO); 

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		// since we skip rendering if minimized, this function doesnt get called so we can uniminimize
		//moved it here so that it can still do its thing 
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		if (width == 0 || height == 0) continue; //skip rendering this frame if minimized

		cout << "fps: " << 1.0f / deltaTime << "\r";

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// upload all pointlights at once via ubo (not to be confused with the adblocker of the same acronym)
		glBindBuffer(GL_UNIFORM_BUFFER, pointLightUBO);
		int numPointLights = std::min((int) pointLights.size(), 10);
		for (int i = 0; i < numPointLights; i++) {
			int offset = i * sizeof(PointLightGPUStruct);
			PointLightGPUStruct pl = pointLights[i].getGPUStruct();
			glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLightGPUStruct), &pl);
		};
		glBufferSubData(GL_UNIFORM_BUFFER, MAX_POINT_LIGHTS * sizeof(PointLightGPUStruct), sizeof(int), &numPointLights);
		

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Scene", &isSceneOpen);
		ImGui::Text("Objects");
		if (ImGui::Button("Add Primitive")) {
			SceneObject sphere("models/sphere.obj");
			sphere.material = std::make_unique<ColorMaterial>(getColorShader(), glm::vec3(0.0f, 0.0f, 1.0f));
			sphere.name = "Sphere " + std::to_string(sceneObjects.size() + 1);

			sceneObjects.push_back(std::move(sphere));
		}
		if (ImGui::Button("Add Backpack")) {
			SceneObject bag("models/backpack/backpack.obj");
			bag.material = std::make_unique<DiffuseMaterial>(getDiffuseShader());
			bag.name = "Backpack " + std::to_string(sceneObjects.size() + 1);
			sceneObjects.push_back(std::move(bag));
		}
		if (ImGui::Button("Add Point Light")) {
			PointLight light(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));


			light.name = "Light " + std::to_string(pointLights.size() + 1);
			pointLights.push_back(std::move(light));
		}

		// rebuild this every frame so that the UI is united
		std::vector<const char*> labels;
		std::vector<Selectable*> selectables;

		// group the objects by shader (rebuilding it every frame)
		// simple solution for now, might make this better later
		// needs to come after the last menu because a sphere could be added to the sceneObjects,
		// something something vector reallocation, dangling pointers, program crashes
		shaderGroups.clear();
		for (SceneObject& obj: sceneObjects) {
			selectables.push_back(&obj);
			labels.push_back(obj.name.c_str());
			
			unsigned int shaderID = obj.material->shader.program;
			shaderGroups[shaderID].push_back(&obj);
		}

		for (PointLight& light: pointLights) {
			selectables.push_back(&light);
			labels.push_back(light.name.c_str());
		}


		static int index = 0;
		ImGui::Text("Scene");
		ImGui::ListBox("##SceneListBox", &index, labels.data(), labels.size(), std::min((int) labels.size(), 20));
		ImGui::End();
		

		if (selectables.size() > 0) {
			selectables[index]->drawInspector();
		}

		ImGui::EndFrame();

		

		processInput(window, !io.WantCaptureMouse);

		for (auto& [program, objsWithProgram]: shaderGroups) {
			if (objsWithProgram.size() == 0) continue;

			Shader& shader = objsWithProgram[0]->material->shader;
			shader.use();
			shader.setVec3("camPos", camera.eye);
			shader.setMat4("view", camera.getViewMatrix());
			shader.setMat4("projection", camera.getProjectionMatrix());

			shader.setFloat("pointLight.constant",  1.0f);
			shader.setFloat("pointLight.linear",    0.09f);
			shader.setFloat("pointLight.quadratic", 0.032f);
			shader.setVec3("pointLight.position", glm::vec3(0.0f));	
			shader.setVec3("pointLight.diffuse", glm::vec3(1.0f));	
			shader.setVec3("pointLight.ambient", glm::vec3(1.0f));	
			shader.setVec3("pointLight.specular", glm::vec3(1.0f));	

			for (auto obj: objsWithProgram) {
				glm::mat4 modelMatrix = obj->getModelMatrix();
				glm::mat4 normalMatrix = glm::inverse(glm::transpose(modelMatrix));
				shader.setBool("hasDiffuseMap", false);
				shader.setBool("hasSpecularMap", false); // if the mesh actually has textures, these become true in mesh.render()

				obj->material->shader.setMat4("model", modelMatrix);
				obj->material->shader.setMat3("normalMatrix", normalMatrix);
				obj->material->apply();
				obj->model.render(shader);
			}
		}

		// all lights use the same shader (the basic color shader)
		Shader& colorShader = getColorShader();
		colorShader.use();
		colorShader.setMat4("view", camera.getViewMatrix());
		colorShader.setMat4("projection", camera.getProjectionMatrix());
		for (PointLight& light: pointLights) {
			colorShader.setMat4("model", light.getModelMatrix());
			light.material->apply();
			light.model.render(colorShader);
		}


		glBindVertexArray(0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyPlatformWindows();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}