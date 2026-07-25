#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <cmath>
#include "include/stb_image/stb_image.h"
#include "Shader.h"
#include "Material.h"
#include "Model.h"
using namespace std;
using namespace glm;


// even though there is a PointLightClass, i need a struct so the data can be packed
// and uploaded to the GPU. 
struct alignas(16) PointLightGPUStruct {
	vec4 position;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

// for UI purposes, sceneobjects and lights are the same
// need to be able to have a homogenous vector of one object type that can represent objects and lights
// that way, the ui can have just one index into that vector of both object types
class Selectable {
public:
	std::string name;
	virtual void drawInspector() = 0;
};

class SceneObject: public Selectable {
	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		Model model;
		std::unique_ptr<Material> material;

		SceneObject(std::string modelPath): model(Model(modelPath)) {};

		glm::mat4 getModelMatrix() {
			rotation.x = std::fmod(rotation.x, 360.0f);
			rotation.y = std::fmod(rotation.y, 360.0f);
			rotation.z = std::fmod(rotation.z, 360.0f);
			if (rotation.x < 0.0f) rotation.x += 360.0f;
			if (rotation.y < 0.0f) rotation.y += 360.0f;
			if (rotation.z < 0.0f) rotation.z += 360.0f;

			glm::mat4 modelMatrix = glm::mat4(1.0);
			modelMatrix = glm::translate(modelMatrix, position);

			modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
			
			modelMatrix = glm::scale(modelMatrix, scale);

			return modelMatrix;
		}

		void drawInspector() {
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 500.0f, 0.0f), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(500.0f, 200.0f), ImGuiCond_Always);
			ImGui::Begin("Inspector");
			ImGui::Text(name.c_str());
			ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
			ImGui::DragFloat3("Scale",    glm::value_ptr(scale),    0.1f);

			// if this function returns true, then currentMaterial changed
			static int currentMaterial = material->getMaterialType();
			if (ImGui::Combo("Material", &currentMaterial, materialTypes, IM_COUNTOF(materialTypes))) {
				switch (currentMaterial) {
					case MaterialType::Color:
						material = std::make_unique<ColorMaterial>(getColorShader(), glm::vec3(1.0f));
						break;
					case MaterialType::Diffuse:
						material = std::make_unique<DiffuseMaterial>(getDiffuseShader());
						break;
					case MaterialType::Lit:
						material = std::make_unique<LitMaterial>(getLitShader());
						break;
				}
			}
			
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 500.0f, 200.0f), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Always);
			material->materialSettingsPanel();
		}
};

class PointLight: public Selectable {
public:
	vec3 position;

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	PointLight(vec3 position, vec3 diffuse, vec3 ambient, vec3 specular): 
		diffuse(diffuse),
		ambient(ambient),
		specular(specular),
		position(position) {};

	PointLightGPUStruct getGPUStruct() {
		PointLightGPUStruct p {
			vec4(position, 0.0), vec4(diffuse, 0.0), vec4(ambient, 0.0), vec4(specular, 0.0), constant, linear, quadratic
		};

		return p;
	}

	void drawInspector() {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 500.0f, 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(500.0f, 300.0f), ImGuiCond_Always);
		ImGui::Begin("Inspector");
		ImGui::Text(name.c_str());
		ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
		
		ImGui::ColorEdit4("Diffuse", glm::value_ptr(diffuse), ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Ambient", glm::value_ptr(ambient), ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Specular", glm::value_ptr(specular), ImGuiColorEditFlags_NoInputs);

		ImGui::Text("Attentuation");
		ImGui::DragFloat("Constant", &constant, 0.1f, 1.0f, 100.0f);
		ImGui::DragFloat("Linear", &linear, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Quadratic", &quadratic, 0.001f, 0.0f, 100.0f);
		
		ImGui::End();
	}
};


#endif