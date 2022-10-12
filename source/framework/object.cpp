#include "object.h"

void Object::setRotation(float pitch_deg, float yaw_deg, float roll_deg)
{
	glm::vec3 euler = glm::vec3(glm::radians(pitch_deg), glm::radians(yaw_deg), glm::radians(roll_deg));
	setRotation(glm::quat(euler));
}

void Object::draw()
{
	if (!shader || !mesh)
		return;

	// make transformation matrix
	glm::mat4 t = glm::mat4(1.0f);
	t = glm::translate(t, position);
	t = t * glm::mat4_cast(rotation);
	t = glm::scale(t, scale);

	// set material
	shader->setMat4("model", t);
	shader->setVec3("albedo", color);
	
	// draw mesh
	mesh->draw();
}