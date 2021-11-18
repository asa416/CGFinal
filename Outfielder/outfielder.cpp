#include "outfielder.h"

Outfielder::Outfielder()
{
	speed = 0.05;
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 0.1, 0.0));
	model = glm::scale(model, glm::vec3(0.05, 0.2, 0.05));
	dressColor = glm::vec3(1.0, 0.0, 0.0);
	position = glm::vec4(0.0, 0.0, 0.0, 1.0);
}