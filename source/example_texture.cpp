#include "common.h"
#include "example_texture.h"


constexpr const char* s_textureVertexShader = R"(
#version 450

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}
)";

constexpr const char* s_texturePixelShader = R"(
#version 450
#extension GL_ARB_shading_language_420pack: enable

layout(location = 0) in vec2 TexCoord;
layout(binding = 0) uniform uf_data
{
    float uf_time;
};

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D frogTexture;

void main()
{
    FragColor = texture(frogTexture, TexCoord);
    FragColor.x = mod(uf_time.x, 1.0f);
}
)";

#include "TGATexture.h"
#include "example_texture.h"

ExampleTexture::ExampleTexture() {
    launchTime = OSGetTime();

    // create shader group
    std::string errorLog(1024, '\0');
    GX2VertexShader *vertexShader = GLSL_CompileVertexShader(s_textureVertexShader, errorLog.data(), (int) errorLog.size(), GLSL_COMPILER_FLAG_NONE);
    if (!vertexShader) {
        WHBLogPrintf("Vertex shader compilation failed for texture example: %s", errorLog.data());
        return;
    }
    GX2PixelShader *pixelShader = GLSL_CompilePixelShader(s_texturePixelShader, errorLog.data(), (int) errorLog.size(), GLSL_COMPILER_FLAG_NONE);
    if (!pixelShader) {
        WHBLogPrintf("Pixel shader compilation failed for texture example: %s", errorLog.data());
        return;
    }

    memset(&s_shaderGroup, 0, sizeof(WHBGfxShaderGroup));
    s_shaderGroup.vertexShader = vertexShader;
    s_shaderGroup.pixelShader = pixelShader;
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.vertexShader->program, s_shaderGroup.vertexShader->size);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.pixelShader->program, s_shaderGroup.pixelShader->size);

    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

    WHBGfxInitShaderAttribute(&s_shaderGroup, "aPos", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
    WHBGfxInitShaderAttribute(&s_shaderGroup, "aTexCoord", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
    WHBGfxInitFetchShader(&s_shaderGroup);

    // upload vertex position
    s_positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
    s_positionBuffer.elemSize = 2 * sizeof(float);
    s_positionBuffer.elemCount = 4;
    GX2RCreateBuffer(&s_positionBuffer);
    void *posUploadBuffer = GX2RLockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
    memcpy(posUploadBuffer, s_positionData, s_positionBuffer.elemSize * s_positionBuffer.elemCount);
    GX2RUnlockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);

    // upload texture coords
    s_texCoordBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
    s_texCoordBuffer.elemSize = 2 * sizeof(float);
    s_texCoordBuffer.elemCount = 4;
    GX2RCreateBuffer(&s_texCoordBuffer);
    void *coordsUploadBuffer = GX2RLockBufferEx(&s_texCoordBuffer, GX2R_RESOURCE_BIND_NONE);
    memcpy(coordsUploadBuffer, s_texCoords, s_texCoordBuffer.elemSize * s_texCoordBuffer.elemCount);
    GX2RUnlockBufferEx(&s_texCoordBuffer, GX2R_RESOURCE_BIND_NONE);

    // upload texture
    std::ifstream fs("fs:/vol/content/texture.tga", std::ios::in | std::ios::binary);
    if (!fs.is_open())
        return;
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    s_texture = TGA_LoadTexture((uint8_t *) data.data(), data.size());
    if (s_texture == nullptr)
        return;

    GX2Sampler sampler;
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_LINEAR);
}

void ExampleTexture::Draw() {
    // render texture
    WHBGfxBeginRender();

    WHBGfxBeginRenderTV();
    WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    GX2SetFetchShader(&s_shaderGroup.fetchShader);
    GX2SetVertexShader(s_shaderGroup.vertexShader);
    GX2SetPixelShader(s_shaderGroup.pixelShader);
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

    // update time uniform in uniform block
    float time = (float)OSTicksToMilliseconds((OSGetTime() - launchTime));
    time *= 0.001f;
    s_timePSUniformBlock[0] = _swapF32(time);
    GX2SetPixelUniformBlock(0, sizeof(s_timePSUniformBlock), (void*)s_timePSUniformBlock);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, s_timePSUniformBlock, sizeof(s_timePSUniformBlock));

    GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&s_texCoordBuffer, 1, s_texCoordBuffer.elemSize, 0);
    GX2SetPixelTexture(s_texture, s_shaderGroup.pixelShader->samplerVars[0].location);
    GX2SetPixelSampler(&s_sampler, s_shaderGroup.pixelShader->samplerVars[0].location);

    GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
    WHBGfxFinishRenderTV();

    WHBGfxBeginRenderDRC();
    WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    GX2SetFetchShader(&s_shaderGroup.fetchShader);
    GX2SetVertexShader(s_shaderGroup.vertexShader);
    GX2SetPixelShader(s_shaderGroup.pixelShader);
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

    GX2SetPixelUniformBlock(0, sizeof(s_timePSUniformBlock), (void*)s_timePSUniformBlock);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, s_timePSUniformBlock, sizeof(s_timePSUniformBlock));

    GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&s_texCoordBuffer, 1, s_texCoordBuffer.elemSize, 0);
    GX2SetPixelTexture(s_texture, s_shaderGroup.pixelShader->samplerVars[0].location);
    GX2SetPixelSampler(&s_sampler, s_shaderGroup.pixelShader->samplerVars[0].location);

    GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
    WHBGfxFinishRenderDRC();

    WHBGfxFinishRender();
}

ExampleTexture::~ExampleTexture() {
    GX2RDestroyBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&s_texCoordBuffer, GX2R_RESOURCE_BIND_NONE);
    TGA_UnloadTexture(s_texture);
}