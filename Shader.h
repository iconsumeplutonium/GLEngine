#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int program;

	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vCode;
		std::string fCode;

		std::ifstream vsFile;
		std::ifstream fsFile;

		vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			std::stringstream vsStream, fsStream;

			vsFile.open(vertexPath);
			fsFile.open(fragmentPath);

			vsStream << vsFile.rdbuf();
			fsStream << fsFile.rdbuf();

			vsFile.close();
			fsFile.close();

			vCode = vsStream.str();
			fCode = fsStream.str();
		} catch (std::ifstream::failure e) {
			std::cout << "Error trying to read vertex and fragment shaders (" << vertexPath << ", " << fragmentPath << ")" << std::endl;   
			exit(-1);
		}

		int success;
		char infoLog[512];

		const char* vertexCode = vCode.c_str();
		const char* fragmentCode = fCode.c_str();

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexCode, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "Could not compile vertex shader (" << vertexPath << ")" << std::endl;
			std::cout << infoLog << std::endl;
			exit(-1);
		}
		
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "Could not compile fragment shader (" << fragmentPath << ")" << std::endl;
			std::cout << infoLog << std::endl;
			exit(-1);
		}
		
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "Could not create program (" << vertexPath << ", " << fragmentPath << ")" << std::endl;
			std::cout << infoLog << std::endl;
			exit(-1);
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void use() {
		glUseProgram(program);
	}

	void setBool(const std::string& name, bool v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniform1i(varLocation, (int) v);
	}

	void setInt(const std::string& name, int v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		if (varLocation < 0) std::cout << "warning: could not find " << name << std::endl;
		glUniform1i(varLocation, v);
	}

	void setFloat(const std::string& name, float v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniform1f(varLocation, v);
	}

	void setMat3(const std::string& name, glm::mat3 v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniformMatrix3fv(varLocation, 1, GL_FALSE, glm::value_ptr(v));
	}

	void setMat4(const std::string& name, glm::mat4 v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(varLocation, 1, GL_FALSE, glm::value_ptr(v));
	}
	
	void setVec3(const std::string& name, glm::vec3 v) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniform3fv(varLocation, 1, &v[0]);
	}

	void setVec3(const std::string& name, float x, float y, float z) const {
		unsigned int varLocation = glGetUniformLocation(program, name.c_str());
		glUniform3f(varLocation, x, y, z);
	}
};

#endif