#ifndef GLDebug_h
#define GLDebug_h

#include "lina.h"

class Model;

#include <vector>

class GLDebug {
public:
	static Model* lineModel;

	struct LineVertex {
		Vector3 pos;
		Vector4 color;
	};

	static std::vector<LineVertex> vertices;

	static void init();
	static void line(const Vector3& from, const Vector3& to, const Vector4& color);
	static void aabb(const Vector3& min, const Vector3& max, const Vector4& color);
	static void reset();
	static void buildMesh();
};
#endif
