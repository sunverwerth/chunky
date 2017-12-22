#include "Material.h"
#include "GLProgram.h"
#include "GLContext.h"

#include <glad/glad.h>
#include <string>

extern GLContext gl;

void Material::use() {
	if (alpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_BLEND);
	}

	if (depthTest) {
		gl.enable(GL_DEPTH_TEST);
	}
	else {
		gl.disable(GL_DEPTH_TEST);
	}

	if (depthWrite) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}

	program->use();
	for (int i = 0; i < textures.size(); ++i) {
		gl.bind(textures[i], i);
		std::string name("texture");
		name += std::to_string(i + 1);
		program->setUniform(name.c_str(), i);
	}
}
