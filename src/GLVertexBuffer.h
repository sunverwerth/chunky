#ifndef GLVertexBuffer_H
#define GLVertexBuffer_H

#include <glad/glad.h>

class GLVertexBuffer {
public:
	GLVertexBuffer() {
		glGenBuffers(1, &handle);
	}

	~GLVertexBuffer() {
		glDeleteBuffers(1, &handle);
	}

	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, handle);
	}

	void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void setData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	GLuint handle;
}; 

#endif
