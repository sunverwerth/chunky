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
		Widget(const Vector2& position): position(position) {}
		void generateVertices(const Vector2& offset, std::vector<GUIVertex>& vertices);
		virtual void generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) {}
		static void GUI::Widget::quad(std::vector<GUI::GUIVertex>& vertices, const Vector2& min, const Vector2& size, const Vector2& uv, const Vector2& uvs, const Vector4& color);

		Widget* parent = nullptr;
		std::vector<Widget*> children;
		Vector2 position;
	};

	class Panel : public Widget {
	public:
		Vector2 size;
		Vector4 color = Vector4::white;

		Panel(const Vector2& position, const Vector2& size): Widget(position), size(size) {}
		void generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) override;
	};

	class Label : public Widget {
	public:
		std::string text;
		float size = 8.0f;
		Vector4 color = Vector4::white;

		Label(const Vector2& position, const std::string& text) : Widget(position), text(text) {}
		void generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) override;
	};

	GUI();
	void updateMesh();

	Camera* camera;
	Material*  material;
	GLMesh* mesh;
	Widget* root;
}; 

#endif
