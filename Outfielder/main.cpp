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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitBuffer();

extern std::vector< glm::vec3 > outvertex, outnormal;
extern std::vector< glm::vec2 > outuv;

std::tr1::shared_ptr<Shader> shader;
std::tr1::shared_ptr<Shader> floorShader;
std::tr1::shared_ptr<Outfielder> player;
GLint width, height;
int num_triangle;

float floorVertex[] = {
	-1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 0.0, 0.0,
	1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 1.0, 0.0,
	1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 1.0, 1.0,

	-1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 0.0, 1.0,
	-1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 0.0, 0.0,
	1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 1.0, 1.0
};

glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 lightColor(1.0f);
glm::vec3 lightPos(0.0, 20.0, 0.0);

glm::mat4 floorMat = glm::mat4(1.0f);

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
	floorShader.reset(new Shader("floorvertex.glsl", "floorfragment.glsl"));
	shader.reset(new Shader("playervertex.glsl", "playerfragment.glsl"));
	player.reset(new Outfielder);
	
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -5.0f));
	
	floorMat = glm::scale(floorMat, glm::vec3(10.0f, 0.0f, 10.0f));
	
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	

	glutMainLoop();
}

GLuint vao, vbo_position, vbo_normal;
GLuint floor_vao, floor_vbo;

int w, h, nrChannels;
GLuint texture;

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

	glGenVertexArrays(1, &floor_vao);
	glGenBuffers(1, &floor_vbo);
	glBindVertexArray(floor_vao);
	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertex), floorVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	(*floorShader).use();
	(*floorShader).setVec3("lightPos", lightPos);
	(*floorShader).setVec3("lightColor", lightColor);
	(*floorShader).setVec3("viewPos", cameraPos);
	(*floorShader).setMat4("projection", projection);
	(*floorShader).setMat4("model", floorMat);

	stbi_set_flip_vertically_on_load(true);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data1 = stbi_load("park.png", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
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

	(*floorShader).use();
	(*floorShader).setMat4("view", view);
	int tLocation1 = glGetUniformLocation((*floorShader).ID, "outTexture");
	glUniform1i(tLocation1, 0);
	glBindVertexArray(floor_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}