#ifndef ORBITCONTROLS_H
#define ORBITCONTROLS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include "Camera.h"

const float PI = 3.141592653f;
const float TWO_PI = 2 * PI;

class OrbitControls: public Camera {
public:
	float r = 10.0f;
	float theta = 0.0f;
	float phi = PI / 2.0;

	glm::vec3 eye = glm::vec3(0.0f, 0.0f, r);
	glm::vec3 at = glm::vec3(0.0f);

	float scrollSensitivity = 1.0f;
	bool isLeftClick = false;

	OrbitControls(float fov, float moveSpeed, float mouseSensitivity, float aspectRatio): Camera(fov, moveSpeed, mouseSensitivity, aspectRatio) {
		updateCameraPos(r, theta, phi);
	};

	void processInput(GLFWwindow* window, float deltaTime) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			isLeftClick = true;
		} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			isLeftClick = false;
			firstMouse = true;
		}
		// if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// 	moveForward(deltaTime);
		// }
		// if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// 	moveBackward(deltaTime);
		// }
		// if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// 	moveLeft(deltaTime);
		// }
		// if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		// 	moveRight(deltaTime);
		// }
		// if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// 	moveUp(deltaTime);
		// }
		// if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		// 	moveDown(deltaTime);
		// }
	}

	void updateCameraRotation(GLFWwindow* window, double xPos, double yPos, float deltaTime) {
		if (!isLeftClick) return;

		if (firstMouse) {
			mousePosLastFrame.x = xPos;
			mousePosLastFrame.y = yPos;
			firstMouse = false;
		}

		glm::vec2 offset = glm::vec2(xPos - mousePosLastFrame.x, yPos - mousePosLastFrame.y);
		mousePosLastFrame.x = xPos;
		mousePosLastFrame.y = yPos;

		offset *= mouseSensitivity * deltaTime;

		theta += offset.x;
		phi -= offset.y;

		if (theta > TWO_PI) theta -= TWO_PI;
		if (theta < 0) theta += TWO_PI;

		const float epsilon = 0.01f;
		phi = std::max(std::min(phi, PI - epsilon), epsilon);

		updateCameraPos(r, theta, phi);
	}

	void updateCameraPos(float r, float theta, float phi) {
		eye.x = r * sin(phi) * cos(theta);
		eye.y = r * cos(phi);
		eye.z = r * sin(phi) * sin(theta);

		dir = glm::normalize(glm::vec3(at - eye));
	}

	void onScroll(double yOffset) {
		r -= yOffset * scrollSensitivity;
		updateCameraPos(r, theta, phi);
		// fov = std::max(std::min(fov, 120.0f), 30.0f);
	}

	glm::mat4 getViewMatrix() override {
		return glm::lookAt(eye, at, up);
	}

	// void moveForward(float deltaTime) {
	// 	eye += dir * moveSpeed * deltaTime;
	// }

	// void moveBackward(float deltaTime) {
	// 	eye -= dir * moveSpeed * deltaTime;
	// }

	// void moveLeft(float deltaTime) {
	// 	eye -= right * moveSpeed * deltaTime;
	// }

	// void moveRight(float deltaTime) {
	// 	eye += right * moveSpeed * deltaTime;
	// }

	// void moveUp(float deltaTime) {
	// 	eye += up * moveSpeed * deltaTime;
	// }
	
	// void moveDown(float deltaTime) {
	// 	eye -= up * moveSpeed * deltaTime;
	// }

};

#endif