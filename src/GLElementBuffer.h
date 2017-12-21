#ifndef GLElementBuffer_H
#define GLElementBuffer_H

#include <glad/glad.h>

class GLElementBuffer {
public:
	GLElementBuffer() {
		glGenBuffers(1, &handle);
	}

	~GLElementBuffer() {
		glDeleteBuffers(1, &handle);
	}

	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	}

	void unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void setData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}

	GLuint handle;
}; 

#endif
