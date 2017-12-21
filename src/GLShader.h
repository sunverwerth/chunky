#ifndef GLShader_H
#define GLShader_H

#include <glad/glad.h>
#include <string>

class GLShader {
public:
	GLShader(GLuint handle): handle(handle) {}

	virtual ~GLShader() {
		glDeleteShader(handle);
	}

	GLuint handle;
};

#endif
