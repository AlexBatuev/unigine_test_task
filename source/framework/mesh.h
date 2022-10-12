#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

class Mesh
{
public:
	Mesh();
	Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
	~Mesh();

	// mesh configuration
	void clear();
	void set(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
	const std::vector<Vertex> &getVertices() const { return vertices; }
	const std::vector<unsigned int> &getIndices() const { return indices; }

	// render the mesh
	void draw(GLenum mode = GL_TRIANGLES);

private:
	// mesh data
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;

	// render data
	unsigned int VAO = 0; // vertex arrays object
	unsigned int VBO = 0; // vertex buffer object
	unsigned int EBO = 0; // element buffer object

	// buffer objects/arrays
	void init_buffers();
	void update_buffers();
	void shutdown_buffers();
};

// helpers, built-in meshes
Mesh createPlane();
Mesh createCube();
Mesh createSphere(int stacks = 8, int slices = 16);
