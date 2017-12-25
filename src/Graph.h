#ifndef Graph_h
#define Graph_h

#include "GUI.h"
#include <vector>

class Graph : public GUI::Widget {
public:
    struct Axis {
        GUI::Label* label;
        bool horiz;
        float pos;
        Vector4 col;
    };

    Graph(const Vector2& position, const Vector2& size);

    void addSample(float v);

    void addAxisHorizontal(float y, const std::string& title, const Vector4& color);

    void addAxisVertical(float x, const std::string& title, const Vector4& color);

    void generateOwnVertices(const Vector2& offset, std::vector<GUI::GUIVertex>& vertices) override;

    float* watch = nullptr;
    bool autoscale = false;
    float scale = 1.0f;
    std::vector<Axis> axes;
    float* samples;
    int pos = 0;
};

#endif