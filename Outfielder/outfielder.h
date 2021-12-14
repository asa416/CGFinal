#ifndef OUTFIELDER_H
#define OUTFIELDER_H

#include <memory>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Outfielder
{
private:
	glm::vec3 Pos;
	float speed;
	float height;
	float weight;
	glm::mat4 bodyMat = glm::mat4(1.0f);
	glm::mat4 headMat = glm::mat4(1.0f);
	glm::mat4 lefthandMat = glm::mat4(1.0f);
	glm::mat4 righthandMat = glm::mat4(1.0f);
	glm::mat4 leftlegMat = glm::mat4(1.0f);
	glm::mat4 rightlegMat = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 leftrotl = glm::mat4(1.0f);
	glm::mat4 rightrotl = glm::mat4(1.0f);
	glm::mat4 leftroth = glm::mat4(1.0f);
	glm::mat4 rightroth = glm::mat4(1.0f);
	glm::mat4 slideMat = glm::mat4(1.0f);
	glm::vec3 dressColor;
	float lr, rr;
	float ratio;
	bool catching;
	bool sliding;
	int slideCount;

public:
	Outfielder();
	void Move(const glm::vec3&);
	glm::vec3 getColor() const;
	void draw(const std::shared_ptr<Shader>& shader, GLuint vao);
	void draw_mini(const std::shared_ptr<Shader>& shader, GLuint vao);
	void setColor(const std::shared_ptr<Shader>& shader);
	glm::vec3 getPos() const;
	void setCatch();
	bool isCatch();
	bool isSlide();
	void setSlide();
	void stopSlide();
};

#endif