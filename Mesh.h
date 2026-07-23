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
};

struct Texture_ {
	unsigned int id;
	string type;
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
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	};

	void render(Shader& shader) {
		unsigned int diffuseNum = 1;
		unsigned int specularNum = 1;
		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNum++);
			else if (name == "texture_specular")
				number = std::to_string(specularNum++);

			shader.setInt(name + number, i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// std::cout << textures.size() << std::endl;

		// glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);		
	};
};


#endif