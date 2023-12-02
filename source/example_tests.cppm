export module ExampleTests;

constexpr const char* s_integerVertexShader = R"(
#version 450

layout(location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}
)";

constexpr const char* s_integerPixelShader = R"(
#version 450

layout(location = 0) out vec4 FragColor;

void main()
{
    // make sure that loop is dynamic and can't be unrolled
    int LOOP_COUNT = 200 + (gl_FrontFacing?1:0);


	int CHECK_VARIABLE = 0;
    for (int i=0; i<LOOP_COUNT; i++)
    {
        CHECK_VARIABLE++;
    }

    if (CHECK_VARIABLE == 0) {
        // this would only occur if the shader loop code is improperly executed
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    // showing green means that the test(s) have passed
    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
)";

export class ExampleTests : public ExampleBase {
public:
    ExampleTests() {
        std::string errorLog(1024, '\0');
        GX2VertexShader* vertexShader = GLSL_CompileVertexShader(s_integerVertexShader, errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
        if (!vertexShader) {
            WHBLogPrintf("Vertex shader compilation failed for texture example: %s", errorLog.data());
            return;
        }
        GX2PixelShader* pixelShader = GLSL_CompilePixelShader(s_integerPixelShader, errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
        if (!pixelShader) {
            WHBLogPrintf("Pixel shader compilation failed for texture example: %s", errorLog.data());
        }
        memset(&s_shaderGroup, 0, sizeof(WHBGfxShaderGroup));
        s_shaderGroup.vertexShader = vertexShader;
        s_shaderGroup.pixelShader = pixelShader;

        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.vertexShader->program, s_shaderGroup.vertexShader->size);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, s_shaderGroup.pixelShader->program, s_shaderGroup.pixelShader->size);

        WHBGfxInitShaderAttribute(&s_shaderGroup, "aPos", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
        WHBGfxInitFetchShader(&s_shaderGroup);

        // upload vertex position
        s_positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
        s_positionBuffer.elemSize = 2 * sizeof(float);
        s_positionBuffer.elemCount = 4;
        GX2RCreateBuffer(&s_positionBuffer);
        void *posUploadBuffer = GX2RLockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
        memcpy(posUploadBuffer, s_positionData, s_positionBuffer.elemSize * s_positionBuffer.elemCount);
        GX2RUnlockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
    }

    void Draw() override {
        WHBGfxBeginRender();

        // render full screen quad on TV
        WHBGfxBeginRenderTV();
        WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        GX2SetFetchShader(&s_shaderGroup.fetchShader);
        GX2SetVertexShader(s_shaderGroup.vertexShader);
        GX2SetPixelShader(s_shaderGroup.pixelShader);
        GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);

        GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
        WHBGfxFinishRenderTV();

        // render full screen quad on DRC
        WHBGfxBeginRenderDRC();
        WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        GX2SetFetchShader(&s_shaderGroup.fetchShader);
        GX2SetVertexShader(s_shaderGroup.vertexShader);
        GX2SetPixelShader(s_shaderGroup.pixelShader);
        GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

        GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);

        GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
        WHBGfxFinishRenderDRC();

        WHBGfxFinishRender();
    }

    ~ExampleTests() {
        GX2RDestroyBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
    }
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
