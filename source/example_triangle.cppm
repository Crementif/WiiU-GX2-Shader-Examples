#pragma once
#include "common.h"

constexpr const char* s_vertexShader = R"(
#version 400 core

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aPosition;

out vec4 vColor;

void main()
{
    gl_Position = vec4(aPosition);
    vColor = aColor;
}
)";

constexpr const char* s_fragmentShader = R"(
#version 400 core

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
)";

class ExampleTriangle : public ExampleBase {
public:
    ExampleTriangle() {
        // create shader group
        std::string errorLog(1024, '\0');
        GX2VertexShader* vertexShader = GLSL_CompileVertexShader(s_vertexShader, errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
        if (!vertexShader) {
            WHBLogPrintf("Vertex shader compilation failed for triangle example: %s", errorLog.data());
            return;
        }
        GX2PixelShader* pixelShader = GLSL_CompilePixelShader(s_fragmentShader, errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
        if (!pixelShader) {
            WHBLogPrintf("Pixel shader compilation failed for triangle example: %s", errorLog.data());
            return;
        }
        memset(&s_shaderGroup, 0, sizeof(WHBGfxShaderGroup));
        s_shaderGroup.vertexShader = vertexShader;
        s_shaderGroup.pixelShader = pixelShader;
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.vertexShader->program, s_shaderGroup.vertexShader->size);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.pixelShader->program, s_shaderGroup.pixelShader->size);

        WHBGfxInitShaderAttribute(&s_shaderGroup, "aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
        WHBGfxInitShaderAttribute(&s_shaderGroup, "aColor", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
        WHBGfxInitFetchShader(&s_shaderGroup);

        // upload vertex position
        s_positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
        s_positionBuffer.elemSize = 4 * 4;
        s_positionBuffer.elemCount = 3;
        GX2RCreateBuffer(&s_positionBuffer);
        void *posUploadBuffer = GX2RLockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
        memcpy(posUploadBuffer, s_positionData, s_positionBuffer.elemSize * s_positionBuffer.elemCount);
        GX2RUnlockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);

        // upload vertex color
        s_colorBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
        s_colorBuffer.elemSize = 4 * 4;
        s_colorBuffer.elemCount = 3;
        GX2RCreateBuffer(&s_colorBuffer);
        void* colUploadBuffer = GX2RLockBufferEx(&s_colorBuffer, GX2R_RESOURCE_BIND_NONE);
        memcpy(colUploadBuffer, s_colorData, s_colorBuffer.elemSize * s_colorBuffer.elemCount);
        GX2RUnlockBufferEx(&s_colorBuffer, GX2R_RESOURCE_BIND_NONE);
    }

    void Draw() override {
        // animate triangle colors
        float *colors = (float *)GX2RLockBufferEx(&s_colorBuffer, GX2R_RESOURCE_BIND_NONE);
        colors[0] = 1.0f;
        colors[1] = colors[1] >= 1.0f ? 0.0f : (colors[1] + 0.01f);
        colors[2] = colors[2] >= 1.0f ? 0.0f : (colors[2] + 0.01f);
        colors[3] = 1.0f;

        colors[4] = colors[4] >= 1.0f ? 0.0f : (colors[4] + 0.01f);
        colors[5] = 1.0f;
        colors[6] = colors[6] >= 1.0f ? 0.0f : (colors[6] + 0.01f);
        colors[7] = 1.0f;

        colors[8] = colors[8] >= 1.0f ? 0.0f : (colors[8] + 0.01f);
        colors[9] = colors[9] >= 1.0f ? 0.0f : (colors[9] + 0.01f);
        colors[10] = 1.0f;
        colors[11] = 1.0f;
        GX2RUnlockBufferEx(&s_colorBuffer, GX2R_RESOURCE_BIND_NONE);

        // render triangle
        WHBGfxBeginRender();

        WHBGfxBeginRenderTV();
        WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        GX2SetFetchShader(&s_shaderGroup.fetchShader);
        GX2SetVertexShader(s_shaderGroup.vertexShader);
        GX2SetPixelShader(s_shaderGroup.pixelShader);
        GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
        GX2RSetAttributeBuffer(&s_colorBuffer, 1, s_colorBuffer.elemSize, 0);
        GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);
        WHBGfxFinishRenderTV();

        WHBGfxBeginRenderDRC();
        WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        GX2SetFetchShader(&s_shaderGroup.fetchShader);
        GX2SetVertexShader(s_shaderGroup.vertexShader);
        GX2SetPixelShader(s_shaderGroup.pixelShader);
        GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
        GX2RSetAttributeBuffer(&s_colorBuffer, 1, s_colorBuffer.elemSize, 0);
        GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);
        WHBGfxFinishRenderDRC();

        WHBGfxFinishRender();
    }

    ~ExampleTriangle() {
        GX2RDestroyBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
        GX2RDestroyBufferEx(&s_colorBuffer, GX2R_RESOURCE_BIND_NONE);
    }
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
