#include "mob.h"

Mob::Mob() {
	speed = 0.05;
	ypos = 0;
	dir = 1;
	pos = glm::vec3(0.0);
	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, glm::vec3(0.2, 0.3, 0.2));

	scale = glm::mat4(1.0f);
	scale = glm::scale(scale, glm::vec3(0.2, 0.3, 0.4));
}

void Mob::set(float x, float y, float z, float s) {
	speed = s;
	pos = glm::vec3(x, y, z);

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = model * scale;
}

void Mob::move() {
	pos.y += speed * dir;
	if (pos.y > max_y)
		dir *= -1;
	else if (pos.y < 0)
		dir *= -1;

	model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = model * scale;
}

glm::mat4 Mob::getModel() const {
	return model;
}