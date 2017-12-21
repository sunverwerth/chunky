#ifndef GLVertexArray_H
#define GLVertexArray_H

#include "GLVertexBuffer.h"

class GLVertexArray {
public:
	GLVertexArray(GLVertexBuffer* buffer) : buffer(buffer) {
		glGenVertexArrays(1, &handle);
	}

	~GLVertexArray() {
		glDeleteVertexArrays(1, &handle);
	}

	void bind() {
		glBindVertexArray(handle);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	GLVertexBuffer* buffer;
	GLuint handle;

}; 

#endif
