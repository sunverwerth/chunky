#include "GUI.h"
#include "GLMesh.h"

void GUI::Widget::quad(std::vector<GUI::GUIVertex>& vertices, const Vector2& min, const Vector2& size, const Vector2& uv, const Vector2& uvs, const Vector4& color) {
	vertices.push_back({ min + Vector2(size.x, 0), uv + Vector2(uvs.x, 0.0f), color });
	vertices.push_back({ min + Vector2(size.x, size.y), uv + Vector2(uvs.x, uvs.y), color });
	vertices.push_back({ min + Vector2(0, size.y), uv + Vector2(0.0f, uvs.y), color });
	vertices.push_back({ min, uv, color });
}

void GUI::Widget::generateVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) {
	generateOwnVertices(offset, vertices);
	for (auto& ch : children) {
		ch->generateVertices(offset + position, vertices);
	}
}

void GUI::Label::generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) {
	float dtex = 1.0f / 32;
	Vector2 p = Vector2(position.x, position.y - size) + offset;

	for (int i = 0; i < text.size(); ++i) {
		char ch = text[i];
		if (ch == '\n') {
			p.x = position.x;
			p.y -= size;
			continue;
		}
		Vector2 uv(dtex * (ch % 16), 1.0f - dtex * (ch / 16 + 1));

		Vector4 border = Vector4::black;
		for (int x = -1; x < 2; ++x) {
			for (int y = -1; y < 2; ++y) {
				if (x == 0 && y == 0) continue;
				quad(vertices, p + Vector2(x, y), Vector2(size, size), uv, Vector2(dtex, dtex), border);
			}
		}
		quad(vertices, p, Vector2(size, size), uv, Vector2(dtex, dtex), color);

		p.x += size;
	}
}

void GUI::Panel::generateOwnVertices(const Vector2& offset, std::vector<GUIVertex>& vertices) {
	float dtex = 1.0f / 256;

	Vector2 p(position + offset);

	quad(vertices, p + Vector2(0, 0), Vector2(4, 4), Vector2(dtex * 128, 1.0f - dtex * 12), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(0, 4), Vector2(4, size.y - 8), Vector2(dtex * 128, 1.0f - dtex * 8), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(0, size.y - 4), Vector2(4, 4), Vector2(dtex * 128, 1.0f - dtex * 4), Vector2(dtex * 4, dtex * 4), color);

	quad(vertices, p + Vector2(4, 0), Vector2(size.x - 8, 4), Vector2(dtex * 132, 1.0f - dtex * 12), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(4, 4), Vector2(size.x - 8, size.y - 8), Vector2(dtex * 132, 1.0f - dtex * 8), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(4, size.y - 4), Vector2(size.x - 8, 4), Vector2(dtex * 132, 1.0f - dtex * 4), Vector2(dtex * 4, dtex * 4), color);

	quad(vertices, p + Vector2(size.x - 4, 0), Vector2(4, 4), Vector2(dtex * 136, 1.0f - dtex * 12), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(size.x - 4, 4), Vector2(4, size.y - 8), Vector2(dtex * 136, 1.0f - dtex * 8), Vector2(dtex * 4, dtex * 4), color);
	quad(vertices, p + Vector2(size.x - 4, size.y - 4), Vector2(4, 4), Vector2(dtex * 136, 1.0f - dtex * 4), Vector2(dtex * 4, dtex * 4), color);
}

GUI::GUI() {
	root = new Widget(Vector2::zero);

	mesh = new GLMesh({
		{ 2, GL_FLOAT, sizeof(float) },
		{ 2, GL_FLOAT, sizeof(float) },
		{ 4, GL_FLOAT, sizeof(float) },
	});
}

void GUI::updateMesh() {
	std::vector<GUIVertex> vertices;
	root->generateVertices(Vector2::zero, vertices);
	mesh->setVertices(vertices.data(), sizeof(GUIVertex), vertices.size(), GL_STREAM_DRAW);

	std::vector<unsigned int> indices;
	for (int i = 0; i < vertices.size() / 4; ++i) {
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}

	mesh->setIndices(indices.data(), sizeof(unsigned int), indices.size(), GL_STREAM_DRAW);
}
