#ifndef ChunkGenerator_h
#define ChunkGenerator_h

#include "perlin.h"
#include "Chunk.h"

static float smoothstep(float v, float p) {
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
		if (noise) delete[] noise;
		noise = new double[chunkSize*chunkSize];

		for (int z = sz; z < sz + chunkSize; ++z) {
			for (int x = sx; x < sx + chunkSize; ++x) {
				float scale = perlin.noise0_1(0.0005f*x + start.x, 0.0005f*z + start.y) * 50;
				float ground = -50 + perlin2.noise0_1(0.00005f*x + start.x, 0.00005f*z + start.y) * 100;
				float sharpness = 1.0f + pow(perlin2.noise0_1(0.005f*x + start.x, 0.005f*z + start.y), 4) * 100;
				noise[(z - offsetz)*chunkSize + (x - offsetx)] = ground + smoothstep(perlin3.octaveNoise0_1(0.01f*x + start.x, 0.01f*z + start.y, 6), sharpness) * scale;
			}
		}
	}

	ChunkGenerator(unsigned int seed) : perlin(seed), perlin2(seed + 1), perlin3(seed + 2) {
		srand(seed);
		start.x = (float)rand() / 100;
		start.y = (float)rand() / 100;
	}

	virtual ~ChunkGenerator() {}
	virtual BlockType getBlockAt(int x, int y, int z) {
		int top = noise[(z - offsetz)*chunkSize + (x - offsetx)];

		if (y > top) {
			if (y > waterLevel) {
				if (y == top + 1 && (rand() % 300) == 0) {
					//return BlockType::WOOD;
				}
				return BlockType::AIR;
			}
			else return BlockType::WATER;
		}
		else if (y == top) {
			if (y > waterLevel) return BlockType::GRASS;
			else if (y == waterLevel) return BlockType::SAND;
			else return BlockType::DIRT;
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
