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

	bool useIndices = true;
	enum class PrimitiveType {
		TRIANGLES,
		LINES
	} primitiveType = PrimitiveType::TRIANGLES;

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
		for (int i = 0; i < elements.size(); ++i) {
			glVertexAttribPointer(i, elements[i].count, elements[i].type, GL_FALSE, stride, (void*)offset);
			glEnableVertexAttribArray(i);
			offset += elements[i].size * elements[i].count;
		}
	}

	~GLMesh();

	void setIndices(const void* data, size_t size, size_t count, unsigned int usage) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size*count, data, usage);
		numIndices = count;
	}

	void setVertices(const void* data, size_t size, size_t count, unsigned int usage) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size*count, data, usage);
		numVertices = count;
	}

	void draw() {
		glBindVertexArray(vao);
		if (useIndices) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			if (primitiveType == PrimitiveType::TRIANGLES) {
				glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
			}
		}
		else {
			if (primitiveType == PrimitiveType::LINES) {
				glDrawArrays(GL_LINES, 0, numVertices);
			}
		}
	}

	int numVertices;
	int numIndices;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
};

#endif
