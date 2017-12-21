#ifndef GLProgram_H
#define GLProgram_H

#include <glad/glad.h>

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "lina.h"

class GLProgram {
public:
	GLProgram(GLuint handle): handle(handle) {}

	~GLProgram() {
		glDeleteProgram(handle);
	}

	int getUniformLocation(const char* name) {
		return glGetUniformLocation(handle, name);
	}

	void use() {
		glUseProgram(handle);
	}

	void setUniform(const char* name, int val) {
		glUniform1i(glGetUniformLocation(handle, name), val);
	}

	void setUniform(const char* name, float val) {
		glUniform1f(glGetUniformLocation(handle, name), val);
	}

	void setUniform(const char* name, const Matrix4x4& mat) {
		glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, GL_FALSE, (float*)&mat);
	}

	void setUniform(const char* name, const Vector2& vec) {
		glUniform2f(glGetUniformLocation(handle, name), vec.x, vec.y);
	}

	void setUniform(const char* name, const Vector3& vec) {
		glUniform3f(glGetUniformLocation(handle, name), vec.x, vec.y, vec.z);
	}

	void setUniform(const char* name, const Vector4& vec) {
		glUniform4f(glGetUniformLocation(handle, name), vec.x, vec.y, vec.z, vec.w);
	}


	GLuint handle;
};

#endif
