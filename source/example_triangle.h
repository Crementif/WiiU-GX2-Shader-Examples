#pragma once

class ExampleTriangle : public ExampleBase {
public:
    ExampleTriangle();
    void Draw() override;
    ~ExampleTriangle();
private:
    const float s_positionData[16] = {
        1.0f, -1.0f,  0.0f, 1.0f,
        0.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,
    };

    const float s_colorData[16] = {
        1.0f,  0.0f,  0.0f, 1.0f,
        0.0f,  1.0f,  0.0f, 1.0f,
        0.0f,  0.0f,  1.0f, 1.0f,
    };

    struct WHBGfxShaderGroup s_shaderGroup = {};
    struct GX2RBuffer s_colorBuffer = {};
    struct GX2RBuffer s_positionBuffer = {};
};
