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
#include <unordered_map>
using namespace std;

#define here cout << "here" << endl;

class Model {
private:
	unordered_map<std::string, Texture_> texturesLoaded;
	vector<Mesh> meshes;
	string path;
	string directory;

public:
	Model(string path): path(path) {
		directory = path.substr(0, path.find_last_of("/\\"));

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "Something went wrong trying to load " << path << endl;
			cout << importer.GetErrorString() << endl;
			exit(-1);
		}
		
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture_> textures;

		for (int i = 0; i < mesh->mNumVertices; i++) {
			Vertex v;
			v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->mNormals != nullptr) {
				v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			} else {
				v.normal = glm::vec3(0.0f);
				v.tangent = glm::vec3(0.0f);
			}

			if (mesh->mTextureCoords[0]) {
				v.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
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
			// cout << "for this mesh-----------" << endl;
			// int metalRougness = material->GetTextureCount(aiTextureType_GLTF_METALLIC_ROUGHNESS);
			// cout << "there are " << metalRougness << "metallic/roughness textures" << endl;

			// int ao = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
			// cout << "there are " << ao << "ao textures" << endl;

			// int n = material->GetTextureCount(aiTextureType_NORMALS);
			// cout << "there are " << n << "normal textures" << endl;

			// int d = material->GetTextureCount(aiTextureType_DIFFUSE);
			// cout << "there are " << d << "diffuse textures" << endl;

			// int s = material->GetTextureCount(aiTextureType_SPECULAR);
			// cout << "there are " << s << "specular textures" << endl;
			
			// float t = 0.0f;
			// material->Get(AI_MATKEY_METALLIC_FACTOR, t);
			// cout << "metallic factor scalar" << t << endl;
			// cout << "-----" << endl;

			vector<Texture_> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			
			vector<Texture_> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<Texture_> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

		Mesh m(vertices, indices, textures);	
		return m;
	};

	vector<Texture_> loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
		vector<Texture_> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			string matName = string(str.C_Str());

			if (texturesLoaded.contains(matName)) {
				textures.push_back(texturesLoaded[matName]);
				continue;
			}

			Texture_ texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = type;
			texture.path = str.C_Str();
			textures.push_back(texture);
			texturesLoaded[texture.path] = texture; // add to loaded textures
		}

		return textures;
	}  

	unsigned int TextureFromFile(const char* path, const string& directory) {
		string filename = string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrChannels;
		// stbi_set_flip_vertically_on_load(true);
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
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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