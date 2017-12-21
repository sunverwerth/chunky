#ifndef GUI_h
#define GUI_h

#include "lina.h"
#include <vector>

class Camera;
class Material;
class GLMesh;

class GUI {
public:
	struct GUIVertex {
		Vector2 pos;
		Vector2 uv;
		Vector4 col;
	};

	class Widget {
	public:
		void generateVertices(std::vector<GUIVertex>& vertices);
		virtual void generateOwnVertices(std::vector<GUIVertex>& vertices) {}

		Widget* parent = nullptr;
		std::vector<Widget*> children;
		Vector2 position = Vector2::zero;
	};

	class Panel : public Widget {
	};

	class Label : public Widget {
	public:
		std::string text;
		float size = 8.0f;
		Vector4 color = Vector4::white;

		Label(const std::string& text) :text(text) {}
		void generateOwnVertices(std::vector<GUIVertex>& vertices) override;
	};

	GUI();
	void updateMesh();

	Camera* camera;
	Material*  material;
	GLMesh* mesh;
	Widget* root;
}; 

#endif
