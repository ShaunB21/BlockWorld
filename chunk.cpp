#include "chunk.h"

// Generates positions for each block in the chunk
void Chunk::generatePositions(int chunkX, int chunkY) {
	for (int i = 0; i < chunkDepth; i++) {
		for (int j = 0; j < chunkWidth; j++) {
			for (int k = 0; k < chunkWidth; k++) {
				blockPositions[i][j][k] = glm::vec3(0.0f + j + chunkX, 0.0f + (blockHeights[i][j][k]), 0.0f + k + chunkY);
			}

		}
	}
}