#ifndef OBJREAD_H
#define _CRT_SECURE_NO_WARNINGS
#define OBJREAD_H

#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glm/glm.hpp"
#include "GL/glm/ext.hpp"
#include "GL/glm/gtc/matrix_transform.hpp"

int loadObj(const char* filename);
int loadObj_normalize_center(const char* filename);


#endif