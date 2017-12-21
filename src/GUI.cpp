#include "GUI.h"
#include "GLMesh.h"

void GUI::Widget::generateVertices(std::vector<GUIVertex>& vertices) {
	generateOwnVertices(vertices);
	for (auto& ch : children) {
		ch->generateVertices(vertices);
	}
}

void GUI::Label::generateOwnVertices(std::vector<GUIVertex>& vertices) {
	float dtex = 1.0f / 16;
	Vector2 p(position.x, position.y - size);

	for (int i = 0; i < text.size(); ++i) {
		char ch = text[i];
		if (ch == '\n') {
			p.x = position.x;
			p.y -= size;
			continue;
		}
		Vector2 uv(dtex * (ch % 16), 1.0f - dtex * (ch / 16 + 1));

		vertices.push_back({ p + Vector2(size, 0), uv + Vector2(dtex, 0.0f), color });
		vertices.push_back({ p + Vector2(size, size), uv + Vector2(dtex, dtex), color });
		vertices.push_back({ p + Vector2(0, size), uv + Vector2(0.0f, dtex), color });
		vertices.push_back({ p + Vector2(0, 0), uv + Vector2(0.0f, 0.0f), color });

		p.x += size;
	}
}

GUI::GUI() {
	root = new Label("Hello, world!\nDas ist ein Test.");

	mesh = new GLMesh({
		{ 2, GL_FLOAT, sizeof(float) },
		{ 2, GL_FLOAT, sizeof(float) },
		{ 4, GL_FLOAT, sizeof(float) },
	});
}

void GUI::updateMesh() {
	std::vector<GUIVertex> vertices;
	root->generateVertices(vertices);
	mesh->setVertices(sizeof(GUIVertex) * vertices.size(), vertices.data(), GL_STREAM_DRAW);

	std::vector<unsigned int> indices;
	for (int i = 0; i < vertices.size() / 4; ++i) {
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}

	mesh->setIndices(sizeof(unsigned int) * indices.size(), indices.data(), GL_STREAM_DRAW);
}
