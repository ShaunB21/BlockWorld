#include "chunkGenerator.h"
#include "noise.h"
#include "chunk.h"
// Generates chunk
Chunk ChunkGenerator::generateChunk(int seed, int chunkX, int chunkY) {
	srand(seed);
	float freq = 4;
	float amp = 0.1;
	int octaves = 4;

	float offsetX = rand() % 100000;
	float offsetY = rand() % 100000;
	Chunk chunk;

	for (int i = 0; i < chunkDepth; i++) {
		for (int j = 0; j < chunkWidth; j++) {
			for (int k = 0; k < chunkWidth; k++) {
				if (i == 0) {
					float val = 0;
					for (int o = 0; o < octaves; o++) {
						val += perlinValue(((j + (chunkX * chunkWidth)) * freq / 400) + offsetX, ((k + (chunkY * chunkWidth)) * freq / 400) + offsetY) * amp;
					}
					if (val > 1.0f)
						val = 1.0f;
					else if (val < -1.0f)
						val = -1.0f;
					int height = std::round(val * 100);
					chunk.blockHeights[i][j][k] = height;
				}
				else if (i > 0) {
					//int height = chunk.blockHeights[i-1][j][k];
					//height = height - 1;
					//chunk.blockHeights[i][j][k] = height;
				}

			}

		}
	}
	chunk.generatePositions(chunkX * chunkWidth, chunkY * chunkWidth);
	return chunk;
}