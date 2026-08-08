#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "Shader.h"
using namespace std;

#ifndef MESH_H
#define MESH_H

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Texture_ {
	unsigned int id;
	aiTextureType type;
	string path;
};

class Mesh {
private:
	unsigned int VAO, VBO, EBO;

public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture_> textures;

	Mesh(vector<Vertex>& verts, vector<unsigned int>& indices, vector<Texture_>& textures): vertices(verts), indices(indices), textures(textures) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	};

	void render(Shader& shader) {
		// assume it to have no textures at all. if it does, in the loop, these will become true and
		// the texture will be used
		shader.setBool("hasDiffuseMap", false);
		shader.setBool("hasAlbedoMap", false);
		shader.setBool("hasSpecularMap", false);
		shader.setBool("hasNormalMap", false);
		shader.setBool("hasMetalRoughMap", false);

		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			switch (textures[i].type) {
				case aiTextureType_DIFFUSE:
					shader.setBool("hasDiffuseMap", true);
					shader.setBool("hasAlbedoMap", true);
					break;
				case aiTextureType_SPECULAR:
					shader.setBool("hasSpecularMap", true);
					break;
				case aiTextureType_NORMALS:
					shader.setBool("hasNormalMap", true);
					break;
				case aiTextureType_GLTF_METALLIC_ROUGHNESS:
					shader.setBool("hasMetalRoughMap", true);
					break;
			}

			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);		
	};
};


#endif