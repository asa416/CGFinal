#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <memory>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "objRead.h"
#include "outfielder.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitBuffer();

extern std::vector< glm::vec3 > outvertex, outnormal;
extern std::vector< glm::vec2 > outuv;

std::tr1::shared_ptr<Shader> shader;
std::tr1::shared_ptr<Outfielder> player;
GLint width, height;
int num_triangle;


glm::vec3 cameraPos = glm::vec3(0.3f, 2.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 lightColor(1.0f);
glm::vec3 lightPos(1.0, 5.0, 0.0);

void main(int argc, char** argv)
{
	width = 1200;
	height = 800;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("PlayBall");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	Shader s("playervertex.glsl", "playerfragment.glsl");
	shader.reset(new Shader("playervertex.glsl", "playerfragment.glsl"));
	player.reset(new Outfielder);
	
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -5.0f));
	
	InitBuffer();

	

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	

	glutMainLoop();
}

GLuint vao, vbo_position, vbo_normal;



GLvoid InitBuffer()
{
	num_triangle = loadObj("cube.obj");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo_position);
	glGenBuffers(1, &vbo_normal);

	(*shader).use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, outvertex.size() * sizeof(glm::vec3), &outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, outnormal.size() * sizeof(glm::vec3), &outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	(*shader).setVec3("lightPos", lightPos);
	(*shader).setVec3("lightColor", lightColor);
	(*shader).setVec3("viewPos", cameraPos);
	(*shader).setVec3("objectColor", (*player).getColor());
	(*shader).setMat4("projection", projection);
	(*shader).setMat4("model", (*player).getModel());

	glEnable(GL_DEPTH_TEST);
}

GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	(*shader).use();
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	(*shader).setMat4("view", view);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, num_triangle);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}