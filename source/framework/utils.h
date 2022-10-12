#pragma once
#include "mesh.h"

class LineDrawer
{
public:
	LineDrawer(const float *points, size_t count, bool loop);
	LineDrawer(const std::vector<glm::vec3> &points, bool loop);

	void setPoints(const float *points, size_t count, bool loop);
	void setPoints(const std::vector<glm::vec3> &points, bool loop);

	void setColor(const glm::vec3 &color) { this->color = color; }
	void setColor(float r, float g, float b) { color = glm::vec3(r, g, b); }
	const glm::vec3 &getColor() const { return color; }

	void draw();

private:
	Mesh mesh;
	glm::vec3 color = glm::vec3(1.0f);
};