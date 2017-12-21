#ifndef Material_h
#define Material_h

class GLProgram;
class GLTexture;

#include <vector>

class Material {
public:
	void use();

	bool alpha;
	GLProgram* program;
	std::vector<GLTexture*> textures;
}; 

#endif
