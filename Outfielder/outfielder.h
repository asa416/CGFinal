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
	glm::vec3 dressColor;
	glm::mat4 model;
	glm::vec4 position;

public:
	Outfielder();


};

#endif