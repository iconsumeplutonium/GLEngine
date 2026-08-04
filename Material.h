#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "include/stb_image/stb_image.h"
#include <imgui/imgui.h>
#include "Shader.h"
using namespace std;
using namespace glm;

enum MaterialType {
	Color = 0,
	Diffuse,
	Lit,
	BasicPBR
};

const char* materialTypes[] = {"Color Material", "Diffuse Material", "Lit Material", "Basic PBR Material"};

// "lazily initialized accessor function"
// Shader's constructor will only get called once (first time tfunction is called)
// then return sthe same isntance
Shader& getColorShader() {
	static Shader colorShader("shaders/basic/basic.vs", "shaders/basic/basic.fs"); // solid color only
	return colorShader;
}

Shader& getDiffuseShader() {
	static Shader diffuseShader("shaders/diffuse/vertex.vs", "shaders/diffuse/fragment.fs"); // diffuse
	return diffuseShader;
}

Shader& getLitShader() {
	static Shader litShader("shaders/light/vertex.vs", "shaders/light/fragment.fs"); // lighting
	return litShader;
}

Shader& getBasicPBRShader() {
	static Shader basicPBRShader("shaders/basic-pbr/vertex.vs", "shaders/basic-pbr/fragment.fs"); // lighting pbr
	return basicPBRShader;
}

class Material {
	public:
		Shader& shader;
		Material(Shader& shader): shader(shader) {};
		virtual void apply() = 0;
		virtual void materialSettingsPanel() = 0;
		virtual MaterialType getMaterialType() = 0;
};

class ColorMaterial: public Material {
	public:
		glm::vec3 color;

		ColorMaterial(Shader& shader, glm::vec3 color): Material(shader), color(color) {};

		// each shader still needs the model/view/projection matrices to be set
		// each SceneObject's render() function should set the model matrix before rendering
		//a and the view/proj matrices should be set globally in the render loop before anything is called
		void apply() {
			shader.setVec3("color", color);
		}

		void materialSettingsPanel() {
			ImGui::Begin("Color Material Settings");

			ImGui::ColorEdit4("Color", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs);
			
			ImGui::End();
		}

		MaterialType getMaterialType() {
			return MaterialType::Color;
		}
};

class DiffuseMaterial: public Material {
public:
	DiffuseMaterial(Shader& shader): Material(shader) {};

	// this one assumes that the references model already has a diffuse texture, so no uniforms needed to be set here
	void apply() {
		shader.use();
	}

	void materialSettingsPanel() {
		ImGui::Begin("Diffuse Material Settings");			
		ImGui::End();
	}

	MaterialType getMaterialType() {
		return MaterialType::Diffuse;
	}	
};

// when creating a primitive, instantiate the material witht he diffuse/speuclar colors already. 
// hasTexture will bet set to true or false somewhere else? and if its false, the color uniforms will be used. if true
// these will be ignored so uploading wont matter.

// when loading a model, assume it to have textures already, and dont pass in a color
// in mesh.render, if it has no textures, it can pass in a color on its own
class LitMaterial: public Material {
public:
	glm::vec3 diffuseColor = glm::vec3(1.0f);
	glm::vec3 specularColor = glm::vec3(1.0f);
	float specularExp = 20.0f;

	LitMaterial(Shader& shader): Material(shader) {};

	void apply() {
		shader.setVec3("diffuseColor", diffuseColor);
		shader.setVec3("specularColor", diffuseColor);
		shader.setFloat("specularExp", specularExp);
	}

	void materialSettingsPanel() {
		ImGui::Begin("Lit Material Settings");
		ImGui::ColorEdit4("Diffuse", glm::value_ptr(diffuseColor), ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Specular", glm::value_ptr(specularColor), ImGuiColorEditFlags_NoInputs);
		ImGui::DragFloat("Specular Exp", &specularExp, 0.1f, 0.0f, 300.0f, "%.2f", ImGuiColorEditFlags_NoInputs);
		ImGui::End();
	}

	MaterialType getMaterialType() {
		return MaterialType::Lit;
	}
};

class BasicPBRMaterial: public Material {
public:
	vec3 albedo = vec3(0.0f);
	float metallicness = 0.0f;
	float roughness = 0.0f;
	float ao = 0.0f;

	BasicPBRMaterial(Shader& shader): Material(shader) {};

	void apply() {
		shader.setVec3("albedo", albedo);
		shader.setFloat("metallicness", metallicness);
		shader.setFloat("roughness", roughness);
		shader.setFloat("ao", ao);
	}

	void materialSettingsPanel() {
		ImGui::Begin("Basic PBR Material Settings");
		ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_NoInputs);
		ImGui::DragFloat("Metallicness", &metallicness, 0.01f, 0.0f, 1.0f, "%.2f", ImGuiColorEditFlags_NoInputs);
		ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f, "%.2f", ImGuiColorEditFlags_NoInputs);
		ImGui::DragFloat("AO", &ao, 0.01f, 0.0f, 10.0f, "%.2f", ImGuiColorEditFlags_NoInputs);
		ImGui::End();
	}

	MaterialType getMaterialType() {
		return MaterialType::BasicPBR;
	}
};


#endif