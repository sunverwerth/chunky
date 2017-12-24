#ifndef ChunkGenerator_h
#define ChunkGenerator_h

#include "perlin.h"
#include "Chunk.h"

static float ease(float v, float p) {
	if (v > 0.5f) {
		v = (v - 0.5f) * 2;
		return (1.0f - powf(1.0f - v, p))*0.5f + 0.5f;
	}
	else {
		v *= 2;
		return powf(v, p) * 0.5f;
	}
}

class ChunkGenerator {
public:
	const int waterLevel = 0;
	double* noise = nullptr;
	int offsetx;
	int offsetz;
	siv::PerlinNoise perlin;
	siv::PerlinNoise perlin2;
	siv::PerlinNoise perlin3;
	Vector2 start;

	void init(int sx, int sz) {
		offsetx = sx;
		offsetz = sz;
		if (!noise) {
			noise = new double[chunkSize*chunkSize];
		}

		for (int z = sz; z < sz + chunkSize; ++z) {
			for (int x = sx; x < sx + chunkSize; ++x) {
				double scale = pow(perlin.noise0_1(0.0005*x + start.x, 0.0005*z + start.y), 4) * 200;
				double ground = -50 + perlin2.noise0_1(0.0005*x + start.x, 0.0005*z + start.y) * 100;
				double sharpness = 1.0 + pow(perlin2.noise0_1(0.01*x + start.x, 0.01*z + start.y), 4) * 50;
				noise[(z - offsetz)*chunkSize + (x - offsetx)] = ground + ease(perlin3.octaveNoise0_1(0.01*x + start.x, 0.01*z + start.y, 6), sharpness) * scale;
			}
		}
	}

	ChunkGenerator(unsigned int seed) : perlin(seed), perlin2(seed + 1), perlin3(seed + 2) {
		srand(seed);
		start.x = (double)rand() / 100;
		start.y = (double)rand() / 100;
	}

	virtual ~ChunkGenerator() {}
	virtual BlockType getBlockAt(int x, int y, int z) {
		int top = noise[(z - offsetz)*chunkSize + (x - offsetx)];
		bool snow = y > 80;

		if (y > top) {
			if (y > waterLevel) {
				if (!snow && y == top + 1) {
					double trees = perlin.noise0_1(0.0075f*x + start.x, 0.0075f*z + start.y);
					if (trees > 0.5f) {
						trees = (trees - 0.5f) * 2;
						trees *= 0.1f;
						if ((double)rand() / RAND_MAX < trees) return BlockType::WOOD;
					}
				}
				return BlockType::AIR;
			}
			else return BlockType::WATER;
		}
		else if (y == top) {
			if (snow) {
				return BlockType::SNOW;
			}
			else {
				if (y > waterLevel) return BlockType::GRASS;
				else if (y == waterLevel) return BlockType::SAND;
			}
			return BlockType::DIRT;
		}
		else if (y > top - 4) {
			return BlockType::DIRT;
		}
		else {
			auto chance = rand() % 100;
			if (chance == 99) return BlockType::GOLD_ORE;
			else if (chance > 90) return BlockType::IRON_ORE;
			else if (chance > 75) return BlockType::COAL_ORE;
			else return BlockType::STONE;
		}
	}
};

#endif
