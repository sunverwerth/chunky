#ifndef Chunk_h
#define Chunk_h

#include "lina.h"
#include <vector>
class Model;
class ChunkGenerator;
class Material;

enum class BlockType : unsigned char {
	AIR = 0,
	STONE,
	DIRT,
	GRASS,
	PLANKS,
	STEPS,
	CLAY,
	BRICKS,
	TNT,
	COBBLE = 16,
	BEDROCK,
	SAND,
	GRAVEL,
	WOOD,
	GOLD_ORE = 32,
	IRON_ORE,
	COAL_ORE,
	LEAVES = 53,
	WATER = 205
};

const int chunkSize = 32;

class DynamicBlock {
public:
	int x;
	int y;
	int z;
	int power;
};

class Chunk {
public:
	Chunk(int x, int y, int z);
	~Chunk();

	void generateBlocks(ChunkGenerator* gen);
	void generateModel();

	bool isEmpty = true;
	bool isNew = true;
	bool isDirty = true;
	BlockType* blocks = nullptr;
	Model* model = nullptr;
	Model* waterModel = nullptr;
	int gridx;
	int gridy;
	int gridz;
	std::vector<DynamicBlock> liveBlocks;

	static Material* chunkMaterial;
	static Material* waterMaterial;

	struct Vertex {
		Vector3 pos;
		Vector2 uv;
		Vector3 norm;
		Vector4 col;
	};

	BlockType getBlockAt(int x, int y, int z);
	void setBlockAt(int x, int y, int z, BlockType type);
	Vector4 calcLight(int x, int y, int z);
};

#endif
