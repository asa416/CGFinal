#ifndef MOB_H
#define MOB_H

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class Mob {
	float max_y = 0.4;
	float ypos;
	float speed;
	int dir;
	glm::vec3 pos;
	glm::mat4 model;
	glm::mat4 scale;
public:
	Mob();
	void set(float x, float y, float z, float speed);
	void move();
	glm::mat4 getModel() const;
};

# endif