#ifndef GLVertexShader_H
#define GLVertexShader_H

#include "GLShader.h"

#include <string>

class GLVertexShader : public GLShader {
public:
	GLVertexShader(GLuint handle): GLShader(handle) {}
}; 

#endif
