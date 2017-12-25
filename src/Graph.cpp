#include "Graph.h"

Graph::Graph(const Vector2& position, const Vector2& size) : Widget(position, size), samples(new float[(int)size.x]) {
    std::fill(samples, samples + (int)size.x, 0.0f);
}

void Graph::addSample(float v) {
    samples[pos] = v;
    pos = (pos + 1) % (int)size.x;
}

void Graph::addAxisHorizontal(float y, const std::string& title, const Vector4& color) {
    auto label = new GUI::Label(Vector2::zero, title);
    add(label);
    axes.push_back({ label, true, y, color });
}

void Graph::addAxisVertical(float x, const std::string& title, const Vector4& color) {
    auto label = new GUI::Label(Vector2::zero, title);
    add(label);
    axes.push_back({ label, false, x, color });
}

void Graph::generateOwnVertices(const Vector2& offset, std::vector<GUI::GUIVertex>& vertices) {
    if (watch) {
        addSample(*watch);
    }

    quad(vertices, position + offset, size, Vector2(0, 0), Vector2(0, 0), Vector4(0, 0, 0, 0.25));
    for (auto& axis : axes) {
        if (axis.horiz) {
            quad(vertices, position + offset + Vector2(0, axis.pos*scale), Vector2(size.x, 1), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), axis.col);
            axis.label->position = Vector2(0, axis.pos*scale + 4);
        }
        else {
            quad(vertices, position + offset + Vector2(axis.pos, 0), Vector2(1, size.y), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), axis.col);
            axis.label->position = Vector2(axis.pos, 4);
        }
    }
    float min = 999999999;
    float max = -99999999;
    for (int i = 1; i < (int)size.x; ++i) {
        auto p = offset + position + Vector2(i, 0);
        float samp = samples[(pos + i - 1) % (int)size.x];
        float samp2 = samples[(pos + i) % (int)size.x];
        if (samp > max) max = samp;
        if (samp < min) min = samp;
        if (samp > samp2) {
            p.y += samp2 * scale;
            samp = samp - samp2;
        }
        else {
            p.y += samp * scale;
            samp = samp2 - samp;
        }
        samp *= scale;
        quad(vertices, p, Vector2(1, std::max(samp, 1.0f)), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), Vector4(1, 1, 1, 0.75));
    }
    if (autoscale && max > 0) {
        scale = size.y / max;
    }
}