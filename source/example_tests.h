#pragma once

class ExampleTests : public ExampleBase {
public:
    ExampleTests();
    void Draw() override;
    ~ExampleTests();
private:
    const float s_positionData[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f,
    };

    struct WHBGfxShaderGroup s_shaderGroup = {};
    struct GX2RBuffer s_positionBuffer = {};
};