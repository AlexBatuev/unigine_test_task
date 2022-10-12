#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "shader.h"
#include "object.h"

class Engine
{
public:
	static Engine *get();

	bool init(int width, int height, const char *title);
	
	// main loop
	bool isDone();
	void update();
	void render();
	void swap();

	void shutdown();

	// world objects
	Object *createObject();
	Object *createObject(Mesh *mesh);
	size_t getNumObjects() const { return objects.size(); }
	Object *getObject(int index) { return objects[index]; }
	void deleteObject(int index);
	void deleteObject(Object *obj);

	// material
	Shader &getShader() { return shader; }

	// environment
	void setEnvironmentColor(const glm::vec3 &color) { envColor = color; }
	const glm::vec3 &getEnvironmentColor() const { return envColor; }

	// light
	void setLightDirection(const glm::vec3 &dir) { lightDir = dir; }
	const glm::vec3 &getLightDirection() const { return lightDir; }
	void setLightColor(const glm::vec3 &color) { lightColor = color; }
	const glm::vec3 &getLightColor() const { return lightColor; }
	void setLightAmbientColor(const glm::vec3 &color) { lightAmbient = color; }
	const glm::vec3 &getLightAmbientColor() const { return lightAmbient; }

	// camera
	Camera &getCamera() { return camera; }
	void setCameraSpeed(float speed) { cam_speed = speed; }
	float getCameraSpeed() const { return cam_speed; }

	// timing
	// return time in seconds since the last frame
	float getDeltaTime() const { return deltaTime; }
	// return time in seconds since start the application
	float getTime() const { return static_cast<float>(glfwGetTime()); }

private:
	static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	static void processInput(GLFWwindow *window);

	// window
	GLFWwindow *window = nullptr;
	float window_width;
	float window_height;

	// camera
	Camera camera;
	float lastX;
	float lastY;
	bool firstMouse = true;
	float cam_speed = SPEED;

	// objects
	Shader shader;
	std::vector<Object *> objects;

	// environment
	glm::vec3 envColor;

	// lighting
	glm::vec3 lightDir;
	glm::vec3 lightColor;
	glm::vec3 lightAmbient;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};
