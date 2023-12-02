#include "common.h"
#include "navigation.h"

#include "example_triangle.h"
#include "example_texture.h"
#include "example_tests.h"

int main() {
    WHBProcInit();
    WHBLogCafeInit();
    romfsInit();
    initializeInputs();
    WHBGfxInit();

    // initialize shader compiler module
    GLSL_Init();

    OSTime lastInput = OSGetTime();
    size_t currShaderIdx = 0;

    {
        std::vector<std::unique_ptr<ExampleBase>> g_shaders;
        g_shaders.emplace_back(std::make_unique<ExampleTriangle>());
        g_shaders.emplace_back(std::make_unique<ExampleTexture>());
        g_shaders.emplace_back(std::make_unique<ExampleTests>());

        while (WHBProcIsRunning()) {
            updateInputs();
            if (pressedBack()) {
                SYSLaunchMenu();
            }
            else if (((lastInput + (OSTime)OSMillisecondsToTicks(800)) < OSGetTime()) && pressedOk()) {
                lastInput = OSGetTime();
                currShaderIdx = (currShaderIdx + 1) % g_shaders.size();
            }

            g_shaders[currShaderIdx]->Draw();
        }
    }

    // shutdown shader compiler module
    // todo: .rpl files using wut have a bug where they aren't properly unloaded so leave the title switching to do the unloading for now
    //GLSL_Shutdown();

    WHBGfxShutdown();
    shutdownInputs();
    romfsExit();
    WHBLogCafeDeinit();
    WHBProcShutdown();
}