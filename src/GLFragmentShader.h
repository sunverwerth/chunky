#ifndef GLFragmentShader_H
#define GLFragmentShader_H

#include "GLShader.h"

#include <string>

class GLFragmentShader : public GLShader {
public:
	GLFragmentShader(GLuint handle) : GLShader(handle) {}
};

#endif
