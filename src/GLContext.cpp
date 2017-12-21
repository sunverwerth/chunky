#include "GLContext.h"

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLProgram.h"
#include "GLTexture.h"
#include "GLMesh.h"
#include "lina.h"

#include "stb_image.h"

std::string readFile(const std::string& name);
void error(const std::string& msg);

void GLContext::clearAll(const Vector4& col, double depth) {
	glClearColor(col.r, col.g, col.b, col.a);
	glClearDepth(depth);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLContext::clearColor(const Vector4& col) {
	glClearColor(col.r, col.g, col.b, col.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLContext::clearDepth(double depth) {
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GLContext::viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	width = width;
	height = height;
	glViewport(0, 0, width, height);
}

void GLContext::drawIndexed(GLenum mode, GLsizei count, GLenum type, const void* indices) {
	glDrawElements(mode, count, type, indices);
}

void GLContext::use(GLProgram* program) {
	if (currentProgram == program) return;
	glUseProgram(program->handle);
	currentProgram = program;
}

GLTexture* GLContext::loadTexture(const char* name) {
	unsigned int handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);
	if (!data) {
		glDeleteTextures(1, &handle);
		return nullptr;
	}

	unsigned int format;
	switch (nrChannels) {
	case 4:
		format = GL_RGBA;
		break;
	case 3:
	default:
		format = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(data);

	auto texture = new GLTexture();
	texture->handle = handle;
	return texture;
}

void GLContext::bind(GLTexture* texture, int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture->handle);
}

void GLContext::enable(unsigned int option) {
	glEnable(option);
}

void GLContext::disable(unsigned int option) {
	glDisable(option);
}

GLProgram* GLContext::createProgram(const char* vsFile, const char* fsFile) {
	auto vs = createVertexShader(vsFile);
	if (!vs) return nullptr;
	auto fs = createFragmentShader(fsFile);
	if (!fs) return nullptr;
	
	auto handle = glCreateProgram();
	glAttachShader(handle, vs->handle);
	glAttachShader(handle, fs->handle);
	glLinkProgram(handle);

	int success;
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (!success) {
		char msg[512];
		glGetProgramInfoLog(handle, sizeof(msg), nullptr, msg);
		error(msg);
		glDeleteProgram(handle);
		return nullptr;
	}
	
	return new GLProgram(handle);
}

GLVertexShader* GLContext::createVertexShader(const char* vsName) {
	auto handle = glCreateShader(GL_VERTEX_SHADER);
	auto src = readFile(vsName);
	auto srcPtr = src.c_str();
	glShaderSource(handle, 1, &srcPtr, nullptr);
	glCompileShader(handle);
	int success;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success) {
		char msg[512];
		glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
		error(msg);
		glDeleteShader(handle);
		return nullptr;
	}

	return new GLVertexShader(handle);
}

GLFragmentShader* GLContext::createFragmentShader(const char* fsName) {
	auto handle = glCreateShader(GL_FRAGMENT_SHADER);
	auto src = readFile(fsName);
	auto srcPtr = src.c_str();
	glShaderSource(handle, 1, &srcPtr, nullptr);
	glCompileShader(handle);
	int success;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success) {
		char msg[512];
		glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
		error(msg);
		glDeleteShader(handle);
		return nullptr;
	}

	return new GLFragmentShader(handle);
}