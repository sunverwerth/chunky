#include "GLDebug.h"
#include "GLMesh.h"
#include "Material.h"
#include "Model.h"
#include "GLContext.h"

extern GLContext gl;

void GLDebug::init() {
	lineModel = new Model();
	lineModel->position = Vector3::zero;
	lineModel->rotation = Quaternion::identity;
	lineModel->material = new Material();
	lineModel->material->alpha = false;
	lineModel->material->program = gl.createProgram("assets/lines_vs.glsl", "assets/lines_fs.glsl");
	lineModel->mesh = new GLMesh({
		{3, GL_FLOAT, sizeof(float)},
		{4, GL_FLOAT, sizeof(float)}
	});
	lineModel->mesh->useIndices = false;
	lineModel->mesh->primitiveType = GLMesh::PrimitiveType::LINES;
}

void GLDebug::line(const Vector3& from, const Vector3& to, const Vector4& color) {
	vertices.push_back({ from, color });
	vertices.push_back({ to, color });
}

void GLDebug::aabb(const Vector3& min, const Vector3& max, const Vector4& color) {
	line(Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z), color);
	line(Vector3(max.x, max.y, min.z), Vector3(max.x, max.y, max.z), color);
	line(Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z), color);
	line(Vector3(min.x, max.y, max.z), Vector3(min.x, max.y, min.z), color);

	line(Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z), color);
	line(Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), color);
	line(Vector3(max.x, min.y, max.z), Vector3(min.x, min.y, max.z), color);
	line(Vector3(min.x, min.y, max.z), Vector3(min.x, min.y, min.z), color);

	line(Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), color);
	line(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color);
	line(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), color);
	line(Vector3(min.x, min.y, max.z), Vector3(min.x, max.y, max.z), color);

}

void GLDebug::reset() {
	vertices.clear();
}

void GLDebug::buildMesh() {
	lineModel->mesh->setVertices(vertices.data(), sizeof(LineVertex), vertices.size(), GL_STREAM_DRAW);
}

Model* GLDebug::lineModel = nullptr;
std::vector<GLDebug::LineVertex> GLDebug::vertices;