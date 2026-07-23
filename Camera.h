#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

class Camera {
protected:
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	float pitch = 0.0f;
	float yaw = -90.0f;

	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glm::vec2 mousePosLastFrame = glm::vec2(400.0f, 300.0f);
	bool firstMouse = true;
	
public:
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::cross(dir, up);

	float fov;
	float moveSpeed;
	float mouseSensitivity;
	float aspectRatio;

	Camera(float fov, float moveSpeed, float mouseSensitivity, float windowWidth, float windowHeight): 
		fov(fov), 
		moveSpeed(moveSpeed), 
		mouseSensitivity(mouseSensitivity),
		aspectRatio(windowWidth / windowHeight),
		mousePosLastFrame(glm::vec2(windowHeight / 2.0f, windowWidth / 2.0f)) {};

	virtual void processInput(GLFWwindow* window, float deltaTime) = 0;
	virtual void updateCameraRotation(GLFWwindow* window, double xPos, double yPos, float deltaTime) = 0;
	virtual void onScroll(double yOffset) = 0;

	virtual glm::mat4 getViewMatrix() {
		return glm::lookAt(eye, eye+dir, up);
	}

	glm::mat4 getProjectionMatrix() {
		return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	}
};

#endif