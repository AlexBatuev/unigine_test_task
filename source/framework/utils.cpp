#include "utils.h"
#include "engine.h"

LineDrawer::LineDrawer(const float *points, size_t count, bool loop)
{
	setPoints(points, count, loop);
}

LineDrawer::LineDrawer(const std::vector<glm::vec3> &points, bool loop)
{
	setPoints(points, loop);
}

void LineDrawer::setPoints(const float *points, size_t count, bool loop)
{
	std::vector<Vertex> v;
	for (int j = 0; j < count; j++)
		v.push_back({glm::vec3(points[j*3], points[j*3+1], points[j*3+2]), glm::vec3(0,1,0), glm::vec2(0)});
	
	std::vector<unsigned int> i;
	for (int j = 1; j < count; j++)
	{
		i.push_back(j-1);
		i.push_back(j);
	}

	if (loop)
	{
		i.push_back(static_cast<unsigned int>(count - 1));
		i.push_back(0);
	}

	mesh.set(v, i);
}

void LineDrawer::setPoints(const std::vector<glm::vec3> &points, bool loop)
{
	std::vector<Vertex> v;
	for (int j = 0; j < points.size(); j++)
		v.push_back({points[j], glm::vec3(0,1,0), glm::vec2(0)});
	
	std::vector<unsigned int> i;
	for (int j = 1; j < points.size(); j++)
	{
		i.push_back(j-1);
		i.push_back(j);
	}

	if (loop)
	{
		i.push_back(static_cast<unsigned int>(points.size() - 1));
		i.push_back(0);
	}

	mesh.set(v, i);
}

void LineDrawer::draw()
{
	Engine *engine = Engine::get();
	engine->getShader().setMat4("model", glm::mat4(1.0f));
	engine->getShader().setVec3("albedo", color);
	mesh.draw(GL_LINES);
}