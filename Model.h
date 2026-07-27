#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "include/stb_image/stb_image.h"
#include "Shader.h"
#include "Mesh.h"
using namespace std;

#define here cout << "here" << endl;

class Model {
private:
	vector<Texture_> texturesLoaded;
	vector<Mesh> meshes;
	string path;
	string directory;

public:
	Model(string path): path(path) {
		// cout << "path is " << path << endl; 
		directory = path.substr(0, path.find_last_of("/\\"));
		// cout << "directory is " << directory << endl; 

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "Something went wrong trying to load " << path << endl;
			cout << importer.GetErrorString() << endl;
			exit(-1);
		}
		
		cout << "in model constructor" << endl;
		processNode(scene->mRootNode, scene);
		cout << "done consturcting model" << endl;
	}

	void processNode(aiNode* node, const aiScene* scene) {
		cout << "processing node" << endl;
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}	

		cout << "processed node" << endl;
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		cout << "processing mesh" << endl;
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture_> textures;

		for (int i = 0; i < mesh->mNumVertices; i++) {
			Vertex v;
			v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->mNormals != nullptr) {
				v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			} else {
				v.normal = glm::vec3(0.0f);
			}

			if (mesh->mTextureCoords[0]) {
				// v.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				glm::vec2 vector;
				vector.x = mesh->mTextureCoords[0][i].x;
				vector.y = mesh->mTextureCoords[0][i].y;
				v.uv = vector;
			}
			else
				v.uv = glm::vec2(0.0f);
			
			vertices.push_back(v);
		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture_> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			
			vector<Texture_> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		Mesh m(vertices, indices, textures);	
		return m;
	};

	vector<Texture_> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
		vector<Texture_> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
				if(std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(texturesLoaded[j]);
					skip = true; 
					// std::cout << "  texture: " << str.C_Str() << " as " << typeName << (skip ? " [CACHED]" : " [NEW]") << std::endl;
					break;
				}
			}
			if (!skip) {
				Texture_ texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				texturesLoaded.push_back(texture); // add to loaded textures

				// std::cout << "  texture: " << str.C_Str() << " as " << typeName << (skip ? " [CACHED]" : " [NEW]") << std::endl;
			}
		}

		return textures;
	}  

	unsigned int TextureFromFile(const char* path, const string& directory) {
		string filename = string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if (!data) {
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
			exit(-1);
		}

		GLenum format = GL_RED;
		switch (nrChannels) {
			case 3: format = GL_RGB; break;
			case 4: format = GL_RGBA; break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		return textureID;
	}

	void render(Shader& shader) {
		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].render(shader);
		}
	}


};

#endif