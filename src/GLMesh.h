#ifndef GLMesh_H
#define GLMesh_H

#include "GLVertexArray.h"
#include "GLElementBuffer.h"

#include <vector>

class GLMesh {
public:
	struct Element {
		unsigned int count;
		unsigned int type;
		size_t size;
	};

	GLMesh(const std::vector<Element>& elements) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		size_t stride = 0;
		for (auto& el : elements) {
			stride += el.count * el.size;
		}

		size_t offset = 0;
		for (int i = 0; i < elements.size();++i) {
			glVertexAttribPointer(i, elements[i].count, elements[i].type, GL_FALSE, stride, (void*)offset);
			glEnableVertexAttribArray(i);
			offset += elements[i].size * elements[i].count;
		}
	}

	~GLMesh() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void setIndices(size_t size, const void* data, unsigned int usage) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
		numIndices = size / sizeof(int);
	}

	void setVertices(size_t size, const void* data, unsigned int usage) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	void draw() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}

	int numIndices;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
};

#endif
