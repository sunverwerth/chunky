#ifndef GLContext_h
#define GLContext_h

#include <glad/glad.h>

class GLTexture;
class GLMesh;
class GLProgram;
class GLVertexShader;
class GLFragmentShader;
struct Vector4;

class GLContext {
public:
	int width;
	int height;
	
	GLProgram* currentProgram = nullptr;

	void clearAll(const Vector4& col, double depth);
	void clearColor(const Vector4& col);
	void clearDepth(double depth);
	void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void drawIndexed(GLenum mode, GLsizei count, GLenum type, const void* indices);
	void use(GLProgram* program);
	GLTexture* loadTexture(const char* name);
	void bind(GLTexture* texture, int slot);
	void enable(unsigned int option);
	void disable(unsigned int option);
	GLProgram* createProgram(const char* vs, const char* fs);
	GLVertexShader* createVertexShader(const char* vs);
	GLFragmentShader* createFragmentShader(const char* fs);
};
#endif
