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
#include <fmod.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<> urd_y{ 0.0, 0.3 };
std::uniform_real_distribution<> urd_speed{ 0.05, 0.15 };

FMOD_SYSTEM* soundSystem;
FMOD_SOUND* bgm;
FMOD_SOUND* soundFile[4];
FMOD_CHANNEL* channel[2];

const float PIE = 3.141592;

class Point {
public:
	float x, z;
	Point(float x, float z) : x(x), z(z) {}
};

enum class DAY {
	SUN = 0,
	MOON
};

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitBuffer();
GLvoid make_circle(const std::vector<glm::vec3>& vec);
void Timer(int value);
void make_spline(float t, unsigned int l);
void Keyboard(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void SpecialUp(int key, int x, int y);
void make_vector(const Point& p, std::vector<std::vector<glm::vec3>>& vec);
void Sound_Setup();
void Data_init();

extern std::vector< glm::vec3 > outvertex, outnormal;
extern std::vector< glm::vec2 > outuv;

std::tr1::shared_ptr<Shader> shader;
std::tr1::shared_ptr<Shader> floorShader;
std::tr1::shared_ptr<Shader> ballShader;
std::tr1::shared_ptr<Shader> bs;
std::tr1::shared_ptr<Outfielder> player;
Mob mobs[100];
GLint width, height;
int num_triangle;
int num_cube;

std::vector<Point> Points;
Point start_point = Point(0.0, 8.0);

int Level;
int now_level;
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
	-10.0f, -6.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	10.0f, -6.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	-10.0f, 14.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	10.0f, 14.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-10.0f, 14.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	10.0f, -6.0f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 8.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 b_cameraPos = glm::vec3(0.0f, 1.0f, 8.0f);
glm::vec3 b_cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 b_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 m_cameraPos = glm::vec3(0.0f, 5.0f, -2.0f);
glm::vec3 m_cameraDirection = glm::vec3(0.0f, 0.0f, -2.0f);
glm::vec3 m_cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);
glm::mat4 m_view = glm::mat4(1.0f);


glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 lightColor(1.0f);
glm::vec3 lightPos(0.0, 10.0, 0.0);
glm::vec3 night_lightPos(3.0, 10.0, 8.0);

glm::mat4 floorMat = glm::mat4(1.0f);
glm::mat4 ballModel;
glm::mat4 bgMat = glm::mat4(1.0f);

glm::mat4 miniMap = glm::mat4(1.0f);

glm::mat4 ballRot = glm::mat4(1.0f);

glm::vec3 ballPos;

glm::vec3 dir(0.0);

std::vector<std::vector<glm::vec3>> endp;

float t = 0;

bool START;
bool MINIMAP;
bool CLEAR;
int clearCount;
DAY day;

float ballSpeed[10] = { 0.015, 0.015, 0.015, 0.02, 0.02, 0.02, 0.02, 0.025, 0.025, 0.025 };

void main(int argc, char** argv)
{
	width = 1200;
	height = 800;

	day = DAY::SUN;

	Level = 10;
	now_level = 0;
	START = false;
	MINIMAP = false;
	CLEAR = false;
	clearCount = 60;

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
	bs.reset(new Shader("bv.glsl", "bf.glsl"));
	player.reset(new Outfielder);
	
	Sound_Setup();

	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -5.0f));
	
	miniMap = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -2.0f, 50.0f);
	m_view = glm::lookAt(m_cameraPos, m_cameraDirection, m_cameraUp);

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

	Data_init();

	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	glutTimerFunc(100, Timer, 1);
	FMOD_System_PlaySound(soundSystem, bgm, NULL, 0, &channel[0]);

	glutMainLoop();
}

GLuint vao, vbo_position, vbo_normal;
GLuint floor_vao, floor_vbo;
GLuint ball_vao, ball_vbo_position, ball_vbo_normal, ball_vbo_uv;
GLuint bg_vao, bg_vbo;
GLuint bvao, bvbo;

int w, h, nrChannels;
GLuint texture;
GLuint btexture;
GLuint bg[3];

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
	(*shader).setMat4("projection", projection);
	(*shader).setFloat("ambientLight", 0.9);

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
	(*floorShader).setFloat("ambientLight", 0.9);

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
	(*ballShader).setFloat("ambientLight", 0.9);
	
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

	glGenTextures(2, bg);
	glBindTexture(GL_TEXTURE_2D, bg[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data3 = stbi_load("backkt.jpg", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data3);

	glBindTexture(GL_TEXTURE_2D, bg[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data4 = stbi_load("back.jpg", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data4);

	glBindTexture(GL_TEXTURE_2D, bg[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data5 = stbi_load("clear.jpg", &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data5);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

GLvoid make_circle(const std::vector<glm::vec3>& vec)
{
	glGenVertexArrays(1, &bvao);
	glGenBuffers(1, &bvbo);

	glBindVertexArray(bvao);
	glBindBuffer(GL_ARRAY_BUFFER, bvbo);
	glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(glm::vec3), &vec[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	(*shader).use();
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	if (t > 0.5) {
		b_cameraDirection = glm::vec3(Points[0].x, 0.0, Points[0].z);
		b_cameraPos = b_cameraDirection + glm::vec3(0.5, 3.0, 3.0);
		view = glm::lookAt(b_cameraPos, b_cameraDirection, b_cameraUp);
	}

	if (CLEAR) {
		(*floorShader).use();
		(*floorShader).setMat4("view", view);
		(*floorShader).setMat4("projection", projection);
		glm::mat4 temp = glm::mat4(1.0f);
		temp = glm::translate(temp, glm::vec3(0.0, -5.0, 0.0));
		(*floorShader).setMat4("model", temp * bgMat);
		int tLocation1 = glGetUniformLocation((*floorShader).ID, "outTexture");
		glUniform1i(tLocation1, 0);
		glBindVertexArray(bg_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bg[2]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glutSwapBuffers();
		return;
	}

	//(*shader).setVec3("viewPos", cameraPos);
	(*shader).setMat4("projection", projection);
	(*shader).setMat4("view", view);
	player.get()->setColor(shader);
	player.get()->draw(shader, vao);

	(*shader).setVec3("objectColor", glm::vec3(0.0, 0.0, 1.0));
	for (int i = 0; i < 100; ++i) {
		(*shader).setMat4("model", mobs[i].getModel());
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	(*floorShader).use();
	(*floorShader).setMat4("view", view);
	(*floorShader).setMat4("projection", projection);
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
	if (day == DAY::SUN)
		glBindTexture(GL_TEXTURE_2D, bg[0]);
	else
		glBindTexture(GL_TEXTURE_2D, bg[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if (START) {
		(*ballShader).use();
		(*ballShader).setMat4("view", view);
		(*ballShader).setMat4("projection", projection);
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
	}

	if (t > 0.5) {
		(*bs).use();
		(*bs).setMat4("projection", projection);
		(*bs).setMat4("view", view);
		make_circle(endp[now_level]);
		glBindVertexArray(bvao);
		glLineWidth(2.0);
		glDrawArrays(GL_LINES, 0, 72);
	}
	// ------------------ ¹Ì´Ï¸Ê ---------------------------------
	if (MINIMAP) {
		glViewport(width - 300, height - 200, 300, 200);

		glm::mat4 sc = glm::mat4(1.0f);
		sc = glm::scale(sc, glm::vec3(2.0, 2.0, 2.0));

		(*floorShader).use();
		(*floorShader).setMat4("projection", miniMap);
		(*floorShader).setMat4("view", m_view);
		(*floorShader).setMat4("model", floorMat);
		tLocation1 = glGetUniformLocation((*floorShader).ID, "outTexture");
		glUniform1i(tLocation1, 0);
		glBindVertexArray(floor_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		(*shader).use();
		(*shader).setMat4("projection", miniMap);
		(*shader).setMat4("view", m_view);
		player.get()->setColor(shader);
		player.get()->draw_mini(shader, vao);

		if (START) {
			(*ballShader).use();
			(*ballShader).setMat4("projection", miniMap);
			(*ballShader).setMat4("view", m_view);
			ballModel = ballModel * sc;
			(*ballShader).setMat4("model", ballModel);
			tLocation1 = glGetUniformLocation((*ballShader).ID, "outTexture");
			glUniform1i(tLocation1, 0);
			glBindVertexArray(ball_vao);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, btexture);
			glDrawArrays(GL_TRIANGLES, 0, num_triangle);
		}
		(*bs).use();
		(*bs).setMat4("projection", miniMap);
		(*bs).setMat4("view", m_view);
		make_circle(endp[now_level]);
		glBindVertexArray(bvao);
		glLineWidth(5.0);
		glDrawArrays(GL_LINES, 0, 72);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
		FMOD_System_Release(soundSystem);
		glutLeaveMainLoop();
		break;
	case 32:
		if (!START) {
			START = true;
			FMOD_System_PlaySound(soundSystem, soundFile[0], NULL, 0, &channel[1]);
		}
		break;
	case 9:
		if (MINIMAP) {
			MINIMAP = false;
		}
		else {
			MINIMAP = true;
		}
		break;
	case '1':
		day = DAY::SUN;
		(*shader).use();
		(*shader).setFloat("ambientLight", 0.9);
		(*shader).setVec3("lightPos", lightPos);
		(*floorShader).use();
		(*floorShader).setFloat("ambientLight", 0.9);
		(*floorShader).setVec3("lightPos", lightPos);
		(*ballShader).use();
		(*ballShader).setFloat("ambientLight", 0.9);
		(*ballShader).setVec3("lightPos", lightPos);
		break;
	case '2':
		day = DAY::MOON;
		(*shader).use();
		(*shader).setFloat("ambientLight", 0.3);
		(*shader).setVec3("lightPos", night_lightPos);
		(*floorShader).use();
		(*floorShader).setFloat("ambientLight", 0.3);
		(*floorShader).setVec3("lightPos", night_lightPos);
		(*ballShader).use();
		(*ballShader).setFloat("ambientLight", 0.3);
		(*ballShader).setVec3("lightPos", night_lightPos);
		break;
	case 's':
	case 'S':

		break;
	case 'a':
	case 'A':
		player.get()->setSlide();
		break;
	}
	glutPostRedisplay();
}


float rad = 5.0;

void Timer(int value) {
	for (int i = 0; i < 100; ++i) {
		mobs[i].move();
	}
	ballRot = glm::mat4(1.0f);
	ballRot = glm::rotate(ballRot, glm::radians(rad), glm::vec3(1.0, 0.0, 0.0));
	rad += 5;
	if (player.get()->isSlide()) {
		player.get()->Move(dir);
	}
	if (START) {
		if (t < 1) {
			make_spline(t, now_level);
			t += ballSpeed[now_level];
			if (t > 0.8) {
				if (!player.get()->isCatch())
					player.get()->setCatch();
			}
		}
		else {
			t = 0;
			START = false;
			glm::vec3 pos = player.get()->getPos();
			player.get()->setCatch();
			float dx, dz;
			dx = pos.x - ballPos.x;
			dz = pos.z - ballPos.z;
			if (dx * dx < 0.5 && dz * dz < 0.5) {
				if (now_level < Level - 1) {
					now_level++;
				}
				else {
					CLEAR = true;
					FMOD_Channel_Stop(channel[0]);
					FMOD_System_PlaySound(soundSystem, soundFile[3], NULL, 0, &channel[1]);
				}
				if (player.get()->isSlide())
					FMOD_System_PlaySound(soundSystem, soundFile[3], NULL, 0, &channel[1]);
				else
					FMOD_System_PlaySound(soundSystem, soundFile[2], NULL, 0, &channel[1]);
			}
			else {
				FMOD_System_PlaySound(soundSystem, soundFile[1], NULL, 0, &channel[1]);
			}
		}
	}
	if (CLEAR) {
		clearCount--;
		if (clearCount < 0) {
			FMOD_System_Release(soundSystem);
			glutLeaveMainLoop();
		}
	}

	glm::vec3 pp = player.get()->getPos();
	//std::cout << "ÁÂÇ¥" << pp.x << ',' << pp.z << '\n';

	glutPostRedisplay();
	glutTimerFunc(100, Timer, 1);
}

void make_spline(float t, unsigned int l)
{
	glm::vec3 mid = (glm::vec3(start_point.x, 0.1, start_point.z) + glm::vec3(Points[l].x, 0.1, Points[l].z)) * glm::vec3(0.5, 0.5, 0.5) + glm::vec3(0.0, heights[l], 0.0);
	ballPos = (1 - t * t) * glm::vec3(start_point.x, 0.1, start_point.z) + 2 * t * (1 - t) * mid + t * t * glm::vec3(Points[l].x, 0.1, Points[l].z);
}

void Special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		dir.x = -1.0;
		break;
	case GLUT_KEY_RIGHT:
		dir.x = 1.0;
		break;
	case GLUT_KEY_UP:
		dir.z = -1.0;
		break;
	case GLUT_KEY_DOWN:
		dir.z = 1.0;
		break;
	}
	if (!player.get()->isSlide())
		player.get()->Move(dir);
	glutPostRedisplay();
}

void SpecialUp(int key, int x, int y)
{
	if (!player.get()->isSlide()) {
		switch (key) {
		case GLUT_KEY_LEFT:
			dir.x = 0.0;
			break;
		case GLUT_KEY_RIGHT:
			dir.x = 0.0;
			break;
		case GLUT_KEY_UP:
			dir.z = 0.0;
			break;
		case GLUT_KEY_DOWN:
			dir.z = 0.0;
			break;
		}
		player.get()->Move(dir);
		glutPostRedisplay();
	}
}

void make_vector(const Point& p, std::vector<std::vector<glm::vec3>>& vec)
{
	std::vector<glm::vec3> temp;
	for (int i = 0; i <= 360; i += 5) {
		float rad = i * PIE / 180;
		float x = p.x + 0.1 * cos(rad);
		float y = 0.0;
		float z = p.z + 0.1 * sin(rad);
		glm::vec3 v = glm::vec3(x, y, z);
		temp.push_back(v);
	}
	vec.push_back(temp);
	temp.clear();
}

void Sound_Setup()
{
	FMOD_System_Create(&soundSystem, FMOD_VERSION);
	FMOD_System_Init(soundSystem, 32, FMOD_INIT_NORMAL, NULL);

	FMOD_System_CreateSound(soundSystem, "bat.mp3", FMOD_DEFAULT, 0, &soundFile[0]);
	FMOD_System_CreateSound(soundSystem, "fail.mp3", FMOD_DEFAULT, 0, &soundFile[1]);
	FMOD_System_CreateSound(soundSystem, "catch.mp3", FMOD_DEFAULT, 0, &soundFile[2]);
	FMOD_System_CreateSound(soundSystem, "slidingcatch.mp3", FMOD_DEFAULT, 0, &soundFile[3]);
	FMOD_System_CreateSound(soundSystem, "bgm.mp3", FMOD_LOOP_NORMAL, 0, &bgm);
}

void Data_init()
{
	Points.push_back(Point(0.0, -4.0));
	make_vector(Points[0], endp);
	heights.push_back(6.0);

	Points.push_back(Point(2.0, -5.0));
	make_vector(Points[1], endp);
	heights.push_back(5.0);

	Points.push_back(Point(-2.0, -3.5));
	make_vector(Points[2], endp);
	heights.push_back(5.0);

	Points.push_back(Point(3.0, -4.5));
	make_vector(Points[3], endp);
	heights.push_back(4.0);

	Points.push_back(Point(-2.0, -4.0));
	make_vector(Points[4], endp);
	heights.push_back(5.0);

	Points.push_back(Point(3.0, -3.5));
	make_vector(Points[5], endp);
	heights.push_back(4.0);

	Points.push_back(Point(-1.5, -3.0));
	make_vector(Points[6], endp);
	heights.push_back(5.0);

	Points.push_back(Point(3.0, -4.0));
	make_vector(Points[7], endp);
	heights.push_back(3.0);

	Points.push_back(Point(-3.0, -3.5));
	make_vector(Points[8], endp);
	heights.push_back(5.0);

	Points.push_back(Point(3.0, -5.0));
	make_vector(Points[9], endp);
	heights.push_back(5.0);
}