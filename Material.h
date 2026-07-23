#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "include/stb_image/stb_image.h"
#include "Shader.h"
using namespace std;

class Material {
	public:
		Shader& shader;
		Material(Shader& shader): shader(shader) {};
		virtual void apply() = 0;
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
};

class DiffuseMaterial: public Material {
public:
	DiffuseMaterial(Shader& shader): Material(shader) {};

	void apply() {
		shader.use();
	}
};


#endif