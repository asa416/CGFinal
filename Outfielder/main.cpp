#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <memory>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "objRead.h"
#include "outfielder.h"
#include "mob.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<> urd_y{ 0.0, 0.3 };
std::uniform_real_distribution<> urd_speed{ 0.05, 0.15 };

class Point {
public:
	float x, z;
	Point(float x, float z) : x(x), z(z) {}
};

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitBuffer();
void Timer(int value);
void make_spline(float t);

extern std::vector< glm::vec3 > outvertex, outnormal;
extern std::vector< glm::vec2 > outuv;

std::tr1::shared_ptr<Shader> shader;
std::tr1::shared_ptr<Shader> floorShader;
std::tr1::shared_ptr<Shader> ballShader;
std::tr1::shared_ptr<Outfielder> player;
Mob mobs[100];
GLint width, height;
int num_triangle;
int num_cube;

std::vector<Point> Points;
Point start_point = Point(0.0, 8.0);

int Level;
std::vector<int> heights;

float floorVertex[] = {
	-1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 0.0, 0.0,
	1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 1.0, 0.0,
	1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 1.0, 1.0,

	-1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 0.0, 1.0,
	-1.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0, 0.0, 0.0,
	1.0f, 0.0f, -1.0f, 0.0, 1.0, 0.0, 1.0, 1.0
};

float bg_vertex[] = {
	-10.0f, -10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	10.0f, -10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	-10.0f, 10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	10.0f, 10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-10.0f, 10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	10.0f, -10.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 8.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 lightColor(1.0f);
glm::vec3 lightPos(0.0, 20.0, 0.0);

glm::mat4 floorMat = glm::mat4(1.0f);
glm::mat4 ballModel;
glm::mat4 bgMat = glm::mat4(1.0f);

glm::mat4 ballRot = glm::mat4(1.0f);

glm::vec3 ballPos;

void main(int argc, char** argv)
{
	width = 1200;
	height = 800;

	Level = 10;

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
	ballShader.reset(new Shader("ballvertex.glsl", "ballfragment.glsl"));
	floorShader.reset(new Shader("floorvertex.glsl", "floorfragment.glsl"));
	shader.reset(new Shader("playervertex.glsl", "playerfragment.glsl"));
	player.reset(new Outfielder);
	
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -5.0f));
	
	floorMat = glm::scale(floorMat, glm::vec3(10.0f, 0.0f, 10.0f));
	
	ballModel = glm::mat4(1.0f);
	ballModel = glm::translate(ballModel, glm::vec3(start_point.x, 0.1, start_point.z));
	ballModel = glm::scale(ballModel, glm::vec3(0.05f, 0.05f, 0.05f));

	float y, z;
	float speed;
	y = 0.2;
	z = -9.5;
	for (int i = 0; i < 100; ++i) {
		speed = urd_speed(dre);
		y = urd_y(dre);
		float x = i / 5.0 - 10;
		mobs[i].set(x, y, z, speed);
	}

	for (int i = 0; i < Level; ++i) {
		Points.push_back(Point(2.0, -4.0));
		heights.push_back(4.0);
	}

	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(100, Timer, 1);
	

	glutMainLoop();
}

GLuint vao, vbo_position, vbo_normal;
GLuint floor_vao, floor_vbo;
GLuint ball_vao, ball_vbo_position, ball_vbo_normal, ball_vbo_uv;
GLuint bg_vao, bg_vbo;

int w, h, nrChannels;
GLuint texture;
GLuint btexture;
GLuint bg;

GLvoid InitBuffer()
{
	num_cube = loadObj("cube.obj");

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

	glGenVertexArrays(1, &bg_vao);
	glGenBuffers(1, &bg_vbo);
	glBindVertexArray(bg_vao);
	glBindBuffer(GL_ARRAY_BUFFER, bg_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bg_vertex), bg_vertex, GL_STATIC_DRAW);
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

	reset();
	num_triangle = loadObj("sphere.obj");
	glGenVertexArrays(1, &ball_vao);
	glGenBuffers(1, &ball_vbo_position);
	glGenBuffers(1, &ball_vbo_normal);
	glGenBuffers(1, &ball_vbo_uv);

	(*ballShader).use();
	glBindVertexArray(ball_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo_position);
	glBufferData(GL_ARRAY_BUFFER, outvertex.size() * sizeof(glm::vec3), &outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, outnormal.size() * sizeof(glm::vec3), &outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, outuv.size() * sizeof(glm::vec3), &outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
	(*ballShader).setVec3("lightPos", lightPos);
	(*ballShader).setVec3("lightColor", lightColor);
	(*ballShader).setVec3("viewPos", cameraPos);
	(*ballShader).setMat4("projection", projection);
	(*ballShader).setMat4("model", ballModel);



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

	glGenTextures(1, &btexture);
	glBindTexture(GL_TEXTURE_2D, btexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data2 = stbi_load("ball.png", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data2);

	glGenTextures(1, &bg);
	glBindTexture(GL_TEXTURE_2D, bg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data3 = stbi_load("backtest.png", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data3);

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
	(*shader).setMat4("model", (*player).getModel());
	(*shader).setVec3("objectColor", (*player).getColor());
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	(*shader).setVec3("objectColor", glm::vec3(0.0, 0.0, 1.0));
	for (int i = 0; i < 100; ++i) {
		(*shader).setMat4("model", mobs[i].getModel());
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	(*floorShader).use();
	(*floorShader).setMat4("view", view);
	(*floorShader).setMat4("model", floorMat);
	int tLocation1 = glGetUniformLocation((*floorShader).ID, "outTexture");
	glUniform1i(tLocation1, 0);
	glBindVertexArray(floor_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	(*floorShader).setMat4("model", bgMat);
	tLocation1 = glGetUniformLocation((*floorShader).ID, "outTexture");
	glUniform1i(tLocation1, 0);
	glBindVertexArray(bg_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bg);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	(*ballShader).use();
	(*ballShader).setMat4("view", view);
	ballModel = glm::mat4(1.0f);
	ballModel = glm::translate(ballModel, ballPos);
	ballModel = glm::scale(ballModel, glm::vec3(0.05, 0.05, 0.05));
	ballModel = ballModel * ballRot;
	(*ballShader).setMat4("model", ballModel);
	tLocation1 = glGetUniformLocation((*ballShader).ID, "outTexture");
	glUniform1i(tLocation1, 0);
	glBindVertexArray(ball_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, btexture);
	glDrawArrays(GL_TRIANGLES, 0, num_triangle);


	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

float t = 0;
float rad = 5.0;

void Timer(int value) {
	for (int i = 0; i < 100; ++i) {
		mobs[i].move();
	}
	ballRot = glm::mat4(1.0f);
	ballRot = glm::rotate(ballRot, glm::radians(rad), glm::vec3(1.0, 0.0, 0.0));
	rad += 5;
	if (t < 1) {
		make_spline(t);
		t += 0.01;
	}
	else {
		t = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(100, Timer, 1);
}

void make_spline(float t)
{
	glm::vec3 mid = (glm::vec3(start_point.x, 0.1, start_point.z) + glm::vec3(Points[0].x, 0.1, Points[0].z)) * glm::vec3(0.5, 0.5, 0.5) + glm::vec3(0.0, heights[0], 0.0);
	ballPos = (1 - t * t) * glm::vec3(start_point.x, 0.1, start_point.z) + 2 * t * (1 - t) * mid + t * t * glm::vec3(Points[0].x, 0.1, Points[0].z);
}