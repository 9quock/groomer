#include "raylib.h"
#include "raymath.h"
#define NOB_IMPLEMENTATION
#include "../nob.h"

#include "../build/shader.h"

#define SCREENSHOT_FILENAME "temp_screenshot.png"

int main(void) {
    const char *screenshot_file = nob_temp_sprintf("%s/"SCREENSHOT_FILENAME, nob_get_current_dir_temp());

    system(TextFormat("grim -t png \"%s\"", screenshot_file));

    InitWindow(0, 0, "Groomer");

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    ToggleFullscreen();

    Camera2D camera = {
        .offset = (Vector2){0.f, 0.f},
        .target = (Vector2){0.f, 0.f},
        .rotation = 0.f,
        .zoom = 1.f,
    }; 

    Shader flashlightShader = LoadShaderFromMemory(0, shader);
    int iResolutionLoc = GetShaderLocation(flashlightShader, "iResolution"); 
    int iTimeLoc = GetShaderLocation(flashlightShader, "iTime"); 
    int mousePosLoc = GetShaderLocation(flashlightShader, "mousePos"); 
    int flashSizeLoc = GetShaderLocation(flashlightShader, "flashSize"); 

    Texture screenshot = LoadTexture(screenshot_file);

    float flashSize = 512;
  
    Camera2D lerpCamera = camera;
    float lerpFlashSize = flashSize;

    while (!WindowShouldClose()) {
        lerpCamera.target = camera.target;
        lerpCamera.offset = camera.offset;
        lerpCamera.zoom += (camera.zoom - lerpCamera.zoom) * GetFrameTime()*2.f;
        lerpFlashSize += (flashSize - lerpFlashSize) * GetFrameTime()*2.f;

        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), lerpCamera);
        Vector2 resolution = {GetScreenWidth(), GetScreenHeight()};
        float iTime = GetTime();

        SetShaderValue(flashlightShader, iResolutionLoc, &resolution, SHADER_UNIFORM_VEC2);
        SetShaderValue(flashlightShader, iTimeLoc, &iTime, SHADER_UNIFORM_FLOAT);
        SetShaderValue(flashlightShader, mousePosLoc, &mouseWorldPos, SHADER_UNIFORM_VEC2);
        SetShaderValue(flashlightShader, flashSizeLoc, &lerpFlashSize, SHADER_UNIFORM_FLOAT);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheelDelta = GetMouseWheelMove();
        if(wheelDelta != 0) {
            float scale = 0.2f*wheelDelta;

            if(IsKeyDown(KEY_LEFT_SHIFT)) {
                flashSize = expf(logf(flashSize)-scale);
            } else {
                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;
                camera.zoom = expf(logf(camera.zoom)+scale);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginShaderMode(flashlightShader);
        BeginMode2D(lerpCamera);
            DrawTexture(screenshot, 0, 0, WHITE);
        EndMode2D();
        EndShaderMode();

        EndDrawing();
    }

    UnloadTexture(screenshot);
    UnloadShader(flashlightShader);
    nob_delete_file(screenshot_file);

    CloseWindow();
    return 0;
}
