#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const int chunkWidth = 10;
const int chunkDepth = 1;
extern const int chunkDepth;
extern const int chunkWidth;
class Chunk {
	public:
		int blockHeights[chunkDepth][chunkWidth][chunkWidth];
		glm::vec3 blockPositions[chunkDepth][chunkWidth][chunkWidth];
		void generatePositions(int chunkX, int chunkY);

};

