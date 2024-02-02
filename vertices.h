// Header file so that external variables can be used by the main class
#pragma once
#define GLEW_STATIC
#include "chunk.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex array
extern GLfloat cubesArray[216];