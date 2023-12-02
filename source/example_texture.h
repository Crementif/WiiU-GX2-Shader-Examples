#pragma once

class ExampleTexture : public ExampleBase {
public:
    ExampleTexture();
    void Draw() override;
    ~ExampleTexture();
private:
    const float s_positionData[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f,
    };

    const float s_texCoords[8] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    struct WHBGfxShaderGroup s_shaderGroup = {};
    struct GX2RBuffer s_positionBuffer = {};
    struct GX2RBuffer s_texCoordBuffer = {};
    struct GX2Texture * s_texture = nullptr;
    struct GX2Sampler s_sampler = {};

    // this NEEDS to be 0x100 aligned
    alignas(0x100) __uint32_t s_timePSUniformBlock[4] = { };
    __int64_t launchTime;
};
