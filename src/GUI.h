#ifndef GUI_h
#define GUI_h

#include "lina.h"
#include <vector>
#include <string>

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
		Widget(const Vector2& position, const Vector2& size): position(position), size(size) {}
		void generateVertices(const Vector2& offset, std::vector<GUIVertex>& vertices);
		virtual void generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) {}
		static void quad(std::vector<GUI::GUIVertex>& vertices, const Vector2& min, const Vector2& size, const Vector2& uv, const Vector2& uvs, const Vector4& color);
		static void line(std::vector<GUI::GUIVertex>& vertices, const Vector2& from, const Vector2& to, const Vector2& uv, const Vector2& uvs, const Vector4& color);
		void add(Widget* child);
		void remove(Widget* child);

		Widget* parent = nullptr;
		Vector2 position = Vector2::zero;
		Vector2 size = Vector2::zero;

	private:
		std::vector<Widget*> children;
	};

	class Panel : public Widget {
	public:
		Vector4 color = Vector4::white;

		Panel(const Vector2& position, const Vector2& size): Widget(position, size) {}
		void generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) override;
	};

	class Label : public Widget {
	public:
		std::string text;
		float fontSize = 8.0f;
		Vector4 color = Vector4::white;

		Label(const Vector2& position, const std::string& text) : Widget(position, Vector2::zero), text(text) {}
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
