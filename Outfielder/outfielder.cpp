#include "outfielder.h"

Outfielder::Outfielder() : speed(0.05), height(0.2), weight(0.1), model(glm::mat4(1.0f)), dressColor(glm::vec3(1.0, 0.0, 0.0)), position(glm::vec3(0.0, 0.0, 0.0))
{
	model = glm::translate(model, glm::vec3(0.0, 0.1, 0.0));
	model = glm::scale(model, glm::vec3(weight, height, weight));
}

void Outfielder::Move(const glm::vec3& dir)
{
	position = position + (dir * glm::vec3(speed, speed, speed));
}

glm::vec3 Outfielder::getColor() const
{
	return dressColor;
}

glm::mat4 Outfielder::getModel() const
{
	return model;
}

//void Outfielder::Draw()
//{
//
//}