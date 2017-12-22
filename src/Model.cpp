#include "Model.h"
#include "GLMesh.h"

Model::~Model() {
	if (mesh) {
		delete mesh;
	}
}
