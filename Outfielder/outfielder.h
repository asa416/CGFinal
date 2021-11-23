#ifndef OUTFIELDER_H
#define OUTFIELDER_H

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class Outfielder
{
private:
	float speed;
	float height;
	float weight;
	glm::vec3 dressColor;
	glm::mat4 model;
	glm::vec3 position;

public:
	Outfielder();
	void Move(const glm::vec3&);
	glm::vec3 getColor() const;
	glm::mat4 getModel() const;
};

#endif