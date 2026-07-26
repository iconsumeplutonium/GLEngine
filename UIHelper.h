#ifndef UIHELPER_H
#define UIHELPER_H

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <cmath>
#include "Shader.h"
#include "Material.h"
#include "Model.h"
#include "SceneObject.h"
using namespace std;
using namespace glm;

namespace UI {
	void DrawFPSPanel(ImGuiIO& io, float displayFPS, float deltaTime) {
		ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - 100), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
		ImGui::Begin("Stats");
		ImGui::Text("FPS: %.1f", displayFPS);
		ImGui::Text("Delta: %.1fms", deltaTime / 1000.0f);
		ImGui::End();
	}

	// void DrawObjectAdderPanel(bool& isSceneOpen, vector<SceneObject>& sceneObjects, vector<PointLight>& pointLights, int MAX_POINT_LIGHTS,) {
	// 	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	// 	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
	// 	ImGui::Begin("Scene", &isSceneOpen);
	// 	ImGui::Text("Objects");
	// 	if (ImGui::Button("Add Sphere")) {
	// 		SceneObject sphere("models/sphere.obj");
	// 		sphere.material = std::make_unique<ColorMaterial>(getColorShader(), glm::vec3(0.0f, 0.0f, 1.0f));
	// 		sphere.name = "Sphere " + std::to_string(sceneObjects.size() + 1);

	// 		sceneObjects.push_back(std::move(sphere));
	// 	}
	// 	if (ImGui::Button("Add Plane")) {
	// 		SceneObject plane("models/plane.obj");
	// 		plane.material = std::make_unique<ColorMaterial>(getColorShader(), glm::vec3(0.0f, 0.0f, 1.0f));
	// 		plane.name = "Plane " + std::to_string(sceneObjects.size() + 1);

	// 		sceneObjects.push_back(std::move(plane));
	// 	}
	// 	if (ImGui::Button("Add Backpack")) {
	// 		SceneObject bag("models/backpack/backpack.obj");
	// 		bag.material = std::make_unique<DiffuseMaterial>(getDiffuseShader());
	// 		bag.name = "Backpack " + std::to_string(sceneObjects.size() + 1);
	// 		sceneObjects.push_back(std::move(bag));
	// 	}

	// 	bool reachedMaxPointLights = pointLights.size() == MAX_POINT_LIGHTS;
	// 	if (reachedMaxPointLights) ImGui::BeginDisabled();

	// 	if (ImGui::Button("Add Point Light")) {
	// 		PointLight light(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
	// 		light.name = "Point Light " + std::to_string(pointLights.size() + 1);
	// 		pointLights.push_back(std::move(light));
	// 	}
	// 	if (reachedMaxPointLights) ImGui::EndDisabled();

	// 	bool reachedMaxSpotlights = spotlights.size() == MAX_SPOTLIGHTS;
	// 	if (reachedMaxSpotlights) ImGui::BeginDisabled();

	// 	if (ImGui::Button("Add Spotlight")) {
	// 		Spotlight light(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
	// 		light.name = "Spotlight " + std::to_string(spotlights.size() + 1);
	// 		spotlights.push_back(std::move(light));
	// 	}
	// 	if (reachedMaxSpotlights) ImGui::EndDisabled();
	// }

};

#endif