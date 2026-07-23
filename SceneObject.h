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

class SceneObject {
	public:
		std::string name;

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
};


#endif