#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

class Camera {
private:
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	glm::vec3 right = glm::cross(dir, up);

	float pitch = 0.0f;
	float yaw = -90.0f;

	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glm::vec2 mousePosLastFrame = glm::vec2(400.0f, 300.0f);
	bool firstMouse = true;
	
public:
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);

	float fov;
	float moveSpeed;
	float mouseSensitivity;
	float aspectRatio;

	Camera(float fov, float moveSpeed, float mouseSensitivity, float aspectRatio): 
		fov(fov), 
		moveSpeed(moveSpeed), 
		mouseSensitivity(mouseSensitivity),
		aspectRatio(aspectRatio) {};

	void processInput(GLFWwindow* window, float deltaTime) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			moveForward(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			moveBackward(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			moveLeft(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			moveRight(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			moveUp(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			moveDown(deltaTime);
		}
	}

	void moveForward(float deltaTime) {
		eye += dir * moveSpeed * deltaTime;
	}

	void moveBackward(float deltaTime) {
		eye -= dir * moveSpeed * deltaTime;
	}

	void moveLeft(float deltaTime) {
		eye -= right * moveSpeed * deltaTime;
	}

	void moveRight(float deltaTime) {
		eye += right * moveSpeed * deltaTime;
	}

	void moveUp(float deltaTime) {
		eye += up * moveSpeed * deltaTime;
	}
	
	void moveDown(float deltaTime) {
		eye -= up * moveSpeed * deltaTime;
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(eye, eye+dir, up);
	}

	glm::mat4 getProjectionMatrix() {
		return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	void updateCameraRotation(double xPos, double yPos, float deltaTime) {
		if (firstMouse) {
			mousePosLastFrame.x = xPos;
			mousePosLastFrame.y = yPos;
			firstMouse = false;
		}

		glm::vec2 offset = glm::vec2(xPos - mousePosLastFrame.x, yPos - mousePosLastFrame.y);
		mousePosLastFrame.x = xPos;
		mousePosLastFrame.y = yPos;

		offset *= mouseSensitivity * deltaTime;

		pitch -= offset.y;
		yaw += offset.x;

		pitch = std::max(std::min(pitch, 89.0f), -89.0f);

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		dir = glm::normalize(direction);
		right = glm::cross(dir, up);
	}

	void onScroll(double yOffset) {
		fov -= yOffset * 0.1f;

		fov = std::max(std::min(fov, 120.0f), 30.0f);
	}

};

#endif