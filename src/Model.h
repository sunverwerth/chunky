#ifndef Model_h
#define Model_h

#include "lina.h"

class GLMesh;
class Material;

class Model {
public:
	GLMesh* mesh = nullptr;
	Material* material = nullptr;
	Vector3 position;
	Quaternion rotation;
	float fade = 1.0f;

	~Model();
};

#endif