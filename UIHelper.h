#ifndef UIHELPER_H
#define UIHELPER_H

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <cmath>
#include <unordered_map>
#include "Shader.h"
#include "Material.h"
#include "Model.h"
#include "SceneObject.h"
using namespace std;
using namespace glm;

struct Primitive {
	string modelPath;
	string displayName;
};

std::unordered_map<string, Primitive> primitives = {
	{"sphere",    Primitive {.modelPath = "models/sphere.obj",    .displayName = "Sphere"   }},
	{"cube",      Primitive {.modelPath = "models/cube.obj",      .displayName = "Cube"     }},
	{"plane",     Primitive {.modelPath = "models/plane.obj",     .displayName = "Plane"    }},
	{"icosphere", Primitive {.modelPath = "models/icosphere.obj", .displayName = "Icosphere"}},
	{"cylinder",  Primitive {.modelPath = "models/cylinder.obj",  .displayName = "Cylinder" }},
	{"cone",      Primitive {.modelPath = "models/cone.obj",      .displayName = "Cone"     }},
	{"torus",     Primitive {.modelPath = "models/torus.obj",     .displayName = "Torus"    }},
};

static bool showWireFrame = false;

namespace UI {
	void DrawFPSPanel(ImGuiIO& io, float displayFPS, float deltaTime) {
		ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - 100), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
		ImGui::Begin("Stats");
		ImGui::Text("FPS: %.1f", displayFPS);
		ImGui::Text("Delta: %.1fms", deltaTime / 1000.0f);
		ImGui::End();
	}

	void DrawPrimitiveAddButton(vector<unique_ptr<Selectable>>& sceneObjects, int& selectedIndex) {
		ImGui::SeparatorText("Stuff");

		if (ImGui::Button("Add Primitive")) {
			ImGui::OpenPopup("primitive popup");
		}

		if (ImGui::BeginPopup("primitive popup")) {
			bool wasSelected = false;
			Primitive selectedPrimitive;

			if (ImGui::Button("Sphere")) {
				wasSelected = true;
				selectedPrimitive = primitives["sphere"];
			}
			if (ImGui::Button("Cube")) {
				wasSelected = true;
				selectedPrimitive = primitives["cube"];
			}
			if (ImGui::Button("Plane")) {
				wasSelected = true;
				selectedPrimitive = primitives["plane"];
			}
			if (ImGui::Button("Icosphere")) {
				wasSelected = true;
				selectedPrimitive = primitives["icosphere"];
			}
			if (ImGui::Button("Cone")) {
				wasSelected = true;
				selectedPrimitive = primitives["cone"];
			}
			if (ImGui::Button("Cylinder")) {
				wasSelected = true;
				selectedPrimitive = primitives["cylinder"];
			}
			if (ImGui::Button("Torus")) {
				wasSelected = true;
				selectedPrimitive = primitives["torus"];
			}


			if (wasSelected) {
				unique_ptr<SceneObject> obj = make_unique<SceneObject>(selectedPrimitive.modelPath);
				obj->material = std::make_unique<ColorMaterial>(getColorShader(), vec3(0.0f, 0.0f, 1.0f));
				obj->name = selectedPrimitive.displayName + " " + std::to_string(sceneObjects.size() + 1);

				sceneObjects.push_back(std::move(obj));
				selectedIndex = sceneObjects.size() - 1;
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
	}


	void DrawAddLightsButton(vector<unique_ptr<Selectable>>& sceneObjects, vector<PointLight*>& pointLights, vector<Spotlight*>& spotlights, int& selectedIndex, const int MAX_POINT_LIGHTS, const int MAX_SPOTLIGHTS) {
		bool reachedMaxPointLights = pointLights.size() == MAX_POINT_LIGHTS;
		bool reachedMaxSpotlights = spotlights.size() == MAX_SPOTLIGHTS;
		
		if (ImGui::Button("Add Light")) {
			ImGui::OpenPopup("light popup");
		}

		if (ImGui::BeginPopup("light popup")) {
			bool wasSelected = false;

			if (reachedMaxPointLights) ImGui::BeginDisabled();
			if (ImGui::Button("Point Light")) {
				wasSelected = true;
				unique_ptr<PointLight> light = make_unique<PointLight>(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
				light->name = "Point Light " + std::to_string(pointLights.size() + 1);

				pointLights.push_back(light.get());
				sceneObjects.push_back(std::move(light));
			}
			if (reachedMaxPointLights) ImGui::EndDisabled();
			


			if (reachedMaxSpotlights) ImGui::BeginDisabled();
			if (ImGui::Button("Spotlight")) {
				wasSelected = true;
				unique_ptr<Spotlight> light = make_unique<Spotlight>(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
				light->name = "Spotlight " + std::to_string(spotlights.size() + 1);
				
				spotlights.push_back(light.get());
				sceneObjects.push_back(std::move(light));
			}
			if (reachedMaxSpotlights) ImGui::EndDisabled();			


			if (wasSelected) {
				selectedIndex = sceneObjects.size() - 1;
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
	}

	void DrawRenderModeSwitch() {
		ImGui::SeparatorText("Render Mode");
		bool renderModeAtStartOfFrame = showWireFrame;

		if (renderModeAtStartOfFrame) ImGui::BeginDisabled();
		if (ImGui::Button("Wireframe")) {
			showWireFrame = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (renderModeAtStartOfFrame) ImGui::EndDisabled();

		ImGui::SameLine();

		if (!renderModeAtStartOfFrame) ImGui::BeginDisabled();
		if (ImGui::Button("Solid")) {
			showWireFrame = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (!renderModeAtStartOfFrame) ImGui::EndDisabled();
	}
};

#endif