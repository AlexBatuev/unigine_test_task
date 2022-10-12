#pragma once

#include "mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Object
{
public:
	Object() {}
	Object(Shader *shader) : shader(shader) {}
	Object(Mesh *mesh, Shader *shader) : mesh(mesh), shader(shader) {}

	// mesh
	void setMesh(Mesh *mesh) { this->mesh = mesh; }
	Mesh *getMesh() const { return mesh; }

	// transformation
	void setPosition(const glm::vec3 &position) { this->position = position; }
	void setPosition(float x, float y, float z) { this->position = glm::vec3(x, y, z); }
	void setRotation(const glm::quat &rotation) { this->rotation = rotation; }
	void setRotation(float pitch_deg, float yaw_deg, float roll_deg);
	void setScale(const glm::vec3 &scale) { this->scale = scale; }
	void setScale(float x, float y, float z) { this->scale = glm::vec3(x, y, z); }
	void setScale(float s) { this->scale = glm::vec3(s); }

	const glm::vec3 &getPosition() const { return position; }
	const glm::quat &getRotation() const { return rotation; }
	const glm::vec3 &getScale() const { return scale; }

	// material parameters
	void setShader(Shader *shader) { this->shader = shader; }
	Shader *getShader() const { return shader; }

	void setColor(const glm::vec3 &color) { this->color = color; }
	void setColor(float r, float g, float b) { color = glm::vec3(r, g, b); }
	const glm::vec3 &getColor() const { return color; }

	// draw the object
	void draw();

private:
	Mesh *mesh = nullptr;
	Shader *shader = nullptr;

	// model transformation
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::quat rotation = glm::quat(1, 0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);

	// material
	glm::vec3 color = glm::vec3(1.0f);
};