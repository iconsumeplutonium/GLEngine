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
#include <imgui/ImGuiFileDialog.h>

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

	void DrawPrimitiveAddButton(vector<shared_ptr<Selectable>>& sceneObjects, int& selectedIndex) {
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
				shared_ptr<SceneObject> obj = make_unique<SceneObject>(selectedPrimitive.modelPath);
				obj->material = std::make_unique<ColorMaterial>(getColorShader(), vec3(0.0f, 0.0f, 1.0f));
				obj->name = selectedPrimitive.displayName + " " + std::to_string(sceneObjects.size() + 1);

				sceneObjects.push_back(std::move(obj));
				selectedIndex = sceneObjects.size() - 1;
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
	}

	void DrawAddLightsButton(vector<shared_ptr<Selectable>>& sceneObjects, vector<weak_ptr<PointLight>>& pointLights, vector<weak_ptr<Spotlight>>& spotlights, vector<weak_ptr<DirectionalLight>>& dirLights, int& selectedIndex, const int MAX_POINT_LIGHTS, const int MAX_SPOTLIGHTS, const int MAX_DIR_LIGHTS) {
		bool reachedMaxPointLights = pointLights.size() == MAX_POINT_LIGHTS;
		bool reachedMaxSpotlights = spotlights.size() == MAX_SPOTLIGHTS;
		bool reachedMaxDirLights = dirLights.size() == MAX_DIR_LIGHTS;
		
		if (ImGui::Button("Add Light")) {
			ImGui::OpenPopup("light popup");
		}

		if (ImGui::BeginPopup("light popup")) {
			bool wasSelected = false;

			if (reachedMaxPointLights) ImGui::BeginDisabled();
			if (ImGui::Button("Point Light")) {
				wasSelected = true;
				shared_ptr<PointLight> light = make_shared<PointLight>(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
				light->name = "Point Light " + std::to_string(pointLights.size() + 1);

				pointLights.push_back(weak_ptr<PointLight>(light));
				sceneObjects.push_back(std::move(light));
			}
			if (reachedMaxPointLights) ImGui::EndDisabled();
			


			if (reachedMaxSpotlights) ImGui::BeginDisabled();
			if (ImGui::Button("Spotlight")) {
				wasSelected = true;
				shared_ptr<Spotlight> light = make_shared<Spotlight>(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
				light->name = "Spotlight " + std::to_string(spotlights.size() + 1);
				
				spotlights.push_back(weak_ptr<Spotlight>(light));
				sceneObjects.push_back(std::move(light));
			}
			if (reachedMaxSpotlights) ImGui::EndDisabled();			



			if (reachedMaxDirLights) ImGui::BeginDisabled();
			if (ImGui::Button("Directional Light")) {
				wasSelected = true;
				shared_ptr<DirectionalLight> light = make_shared<DirectionalLight>(glm::vec3(0.0f, -1.0f, 0.0f));
				light->name = "Directional Light " + std::to_string(dirLights.size() + 1);
				
				dirLights.push_back(weak_ptr<DirectionalLight>(light));
				sceneObjects.push_back(std::move(light));
			}
			if (reachedMaxDirLights) ImGui::EndDisabled();			


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

	void DrawDeleteItemContextMenu(int& markedForDeletion, int& selectedIndex, vector<const char*>& labels) {
		ImGui::SeparatorText("Scene");
		// ImGui::ListBox("##SceneListBox", &selectedIndex, labels.data(), labels.size(), std::min((int) labels.size(), 20));

		if (ImGui::BeginListBox("##SceneListBox")) {
			for (int i = 0; i < labels.size(); i++) {
				bool isSelected = (selectedIndex == i);

				ImGui::PushID(i);
				if (ImGui::Selectable(labels[i], isSelected)) {
					selectedIndex = i;
				}

				if (ImGui::BeginPopupContextItem("ItemContextMenu")) {
					if (ImGui::MenuItem("Delete")) {
						markedForDeletion = i;
					}

					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}
		ImGui::End();
	}

	void DrawAddModelButton(vector<shared_ptr<Selectable>>& sceneObjects, int& selectedIndex) {
		if (ImGui::Button("Add Model")) {
			IGFD::FileDialogConfig config {
				.path = "."
			};
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", config);
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { 
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2, 500.0f), ImGuiCond_Always);
			if (ImGuiFileDialog::Instance()->IsOk()) {
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				// std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();
				
				shared_ptr<SceneObject> model = make_unique<SceneObject>(filePathName);
				model->material = std::make_unique<DiffuseMaterial>(getDiffuseShader());
				model->name = name;
				sceneObjects.push_back(std::move(model));
				selectedIndex = sceneObjects.size() - 1;
			}
			
			ImGuiFileDialog::Instance()->Close();
		}
	}
};

#endif