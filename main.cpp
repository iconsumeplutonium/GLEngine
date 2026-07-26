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
#include "UIHelper.h"
#include <unordered_map>
using namespace std;
	
float fpsRefreshInterval = 0.25f;
float fpsDebounceTimer = 0.0f;
int frameCount = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 70.0f;
const float WINDOW_WIDTH = 1280.0f;
const float WINDOW_HEIGHT = 720.0f;

const int MAX_POINT_LIGHTS = 10;
const int MAX_SPOTLIGHTS = 10;
const int MAX_DIR_LIGHTS = 10;

// FirstPersonControls camera(fov, 2.5f, 50.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
OrbitControls camera(fov, 2.5f, 50.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

// struct Primitive {
// 	string modelPath;
// 	string displayName;
// };

// std::unordered_map<string, Primitive> primitives = {
// 	{"sphere",    Primitive {.modelPath = "models/sphere.obj",    .displayName = "Sphere"   }},
// 	{"cube",      Primitive {.modelPath = "models/cube.obj",      .displayName = "Cube"     }},
// 	{"plane",     Primitive {.modelPath = "models/plane.obj",     .displayName = "Plane"    }},
// 	{"icosphere", Primitive {.modelPath = "models/icosphere.obj", .displayName = "Icosphere"}},
// 	{"cylinder",  Primitive {.modelPath = "models/cylinder.obj",  .displayName = "Cylinder" }},
// 	{"cone",      Primitive {.modelPath = "models/cone.obj",      .displayName = "Cone"     }},
// 	{"torus",     Primitive {.modelPath = "models/torus.obj",     .displayName = "Torus"    }},
// };

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
	
	float displayFPS = 0.0f;
	bool isSceneOpen = true;
	int selectedIndex = 0;
	std::vector<shared_ptr<Selectable>> sceneObjects;
	std::unordered_map<unsigned int, std::vector<shared_ptr<Selectable>>> shaderGroups;

	// need a separate pointer to the lights so taht we can quickly find them and upload their data to the gpu
	vector<weak_ptr<PointLight>> pointLights;
	vector<weak_ptr<Spotlight>> spotlights;
	vector<weak_ptr<DirectionalLight>> dirLights;

	unsigned int lightUBO;
	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	int size = (sizeof(PointLightGPUStruct) * MAX_POINT_LIGHTS) + 
				(sizeof(SpotlightGPUStruct) * MAX_SPOTLIGHTS) +
				(sizeof(DirLightGPUStruct) * MAX_DIR_LIGHTS) + 12; // +4 for each numlights uniform

	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); 
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO); 

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		// since we skip rendering if minimized, this function doesnt get called so we can uniminimize
		//moved it here so that it can still do its thing 
		glfwPollEvents();
		frameCount++;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		fpsDebounceTimer += deltaTime;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		if (width == 0 || height == 0) continue; //skip rendering this frame if minimized

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// filter out the references to deleted lights
		pointLights.erase(std::remove_if(pointLights.begin(), pointLights.end(), [](weak_ptr<PointLight>& weakLight) {
			if (auto test = weakLight.lock()) return false;
			return true;
		}), pointLights.end());
		spotlights.erase(std::remove_if(spotlights.begin(), spotlights.end(), [](weak_ptr<Spotlight>& weakLight) {
			if (auto test = weakLight.lock()) return false;
			return true;
		}), spotlights.end());
		dirLights.erase(std::remove_if(dirLights.begin(), dirLights.end(), [](weak_ptr<DirectionalLight>& weakLight) {
			if (auto test = weakLight.lock()) return false;
			return true;
		}), dirLights.end());

		// upload all lights at once via ubo (not to be confused with the adblocker of the same acronym)
		int baseOffset = 0;
		glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
		int numPointLights = pointLights.size();
		for (int i = 0; i < numPointLights; i++) {
			shared_ptr<PointLight> light = pointLights[i].lock();

			int offset = baseOffset + (i * sizeof(PointLightGPUStruct));
			PointLightGPUStruct pl = light->getGPUStruct();
			glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLightGPUStruct), &pl);
		};

		baseOffset = MAX_POINT_LIGHTS * sizeof(PointLightGPUStruct);
		int numSpotlights = spotlights.size();
		for (int i = 0; i < numSpotlights; i++) {
			shared_ptr<Spotlight> light = spotlights[i].lock();

			int offset = baseOffset + i * sizeof(SpotlightGPUStruct);
			SpotlightGPUStruct pl = light->getGPUStruct();
			glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(SpotlightGPUStruct), &pl);
		};

		baseOffset += MAX_SPOTLIGHTS * sizeof(SpotlightGPUStruct);
		int numDirLights = dirLights.size();
		for (int i = 0; i < numDirLights; i++) {
			shared_ptr<DirectionalLight> light = dirLights[i].lock();

			int offset = baseOffset + i * sizeof(DirLightGPUStruct);
			DirLightGPUStruct dl = light->getGPUStruct();
			glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(DirLightGPUStruct), &dl);
		};

		baseOffset += MAX_SPOTLIGHTS * sizeof(DirLightGPUStruct);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(int), &numPointLights);
		baseOffset += sizeof(int);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(int), &numSpotlights);
		baseOffset += sizeof(int);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(int), &numDirLights);

		
		if (fpsDebounceTimer > fpsRefreshInterval) {
			displayFPS = frameCount / fpsDebounceTimer;
			frameCount = 0;
			fpsDebounceTimer = 0.0f;
		}


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		UI::DrawFPSPanel(io, displayFPS, deltaTime);

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Always);
		ImGui::Begin("Heirarchy");

		UI::DrawRenderModeSwitch();
		UI::DrawPrimitiveAddButton(sceneObjects, selectedIndex);
		UI::DrawAddLightsButton(sceneObjects, pointLights, spotlights, dirLights, selectedIndex, MAX_POINT_LIGHTS, MAX_SPOTLIGHTS, MAX_DIR_LIGHTS);
		

		if (ImGui::Button("Add Backpack")) {
			shared_ptr<SceneObject> bag = make_unique<SceneObject>("models/backpack/backpack.obj");
			bag->material = std::make_unique<DiffuseMaterial>(getDiffuseShader());
			bag->name = "Backpack " + std::to_string(sceneObjects.size() + 1);
			sceneObjects.push_back(std::move(bag));
			selectedIndex = sceneObjects.size() - 1;
		}

		

		std::vector<const char*> labels;

		// group the objects by shader (rebuilding it every frame)
		// simple solution for now, might make this better later
		// needs to come after the last menu because a sphere could be added to the sceneObjects,
		// something something vector reallocation, dangling pointers, program crashes
		shaderGroups.clear();
		for (auto& obj: sceneObjects) {
			labels.push_back(obj->name.c_str());
			
			unsigned int shaderID = obj->getShader().program;
			shaderGroups[shaderID].push_back(obj);
		}

		int markedForDeletion = -1;
		UI::DrawDeleteItemContextMenu(markedForDeletion, selectedIndex, labels);

		// if something was rqeusted to be deleted, get rid of it
		if (markedForDeletion >= 0 && sceneObjects.size() > 0) {
			sceneObjects.erase(sceneObjects.begin() + markedForDeletion);
			selectedIndex = -1;
			markedForDeletion = -1;
		}
		

		if (sceneObjects.size() > 0 && selectedIndex >= 0) {
			sceneObjects[selectedIndex]->drawInspector();
		}

		ImGui::EndFrame();

		

		processInput(window, !io.WantCaptureMouse);

		for (auto& [program, objsWithProgram]: shaderGroups) {
			if (objsWithProgram.size() == 0) continue;

			Shader& shader = objsWithProgram[0]->getShader();
			shader.use();
			shader.setVec3("camPos", camera.eye);
			shader.setMat4("view", camera.getViewMatrix());
			shader.setMat4("projection", camera.getProjectionMatrix());

			for (auto obj: objsWithProgram) {
				obj->render();
			}
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