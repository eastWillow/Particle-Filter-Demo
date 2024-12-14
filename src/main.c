/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you
--  wrote the original software. If you use this software in a product, an acknowledgment
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void DrawEquilateralTriangle(Vector2 center, float radius, Color color)
{
    float sideLength = (6 * radius) / sqrtf(3); // Calculate the side length
    float angleOffset = -PI / 2;                // Start angle to point upwards (90 degrees)

    Vector2 vertices[3];
    for (int i = 0; i < 3; i++)
    {
        float angle = angleOffset - (i * 2 * PI / 3); // -120-degree increments , Vertex must be provided in counter-clockwise order
        vertices[i] = (Vector2){
            center.x + sideLength * cosf(angle) / 2.0f,
            center.y + sideLength * sinf(angle) / 2.0f};
    }

    // Draw the filled triangle
    DrawTriangle(vertices[0], vertices[1], vertices[2], color);
}

int main()
{
    uint16_t winWitdh = 1280;
    uint16_t winHeight = 800;
    Vector2 targetCenter = {0, 0};
    float targetCenterX = winWitdh / 2;
    float targetCenterY = 0;
    bool mouseFollow = true;
    char strBufLeft[] = "9999.99 cm";
    char strBufRight[] = "9999.99 cm";
    float noiseRatio = 0;
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

    // Create the window and OpenGL context
    InitWindow(winWitdh, winHeight, "Partical Filter Demo");

    // game loop
    while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_T))
        {
            mouseFollow = !mouseFollow;
        }
        Rectangle checkBoxPosition = {100, winHeight - 20, 20, 20};
        GuiCheckBox(checkBoxPosition, "Toggle Mouse Tracking (t)", &mouseFollow);
        if (mouseFollow)
        {
            targetCenter = GetMousePosition();
        }
        else
        {
            Rectangle targetSliderBarPos = {100, checkBoxPosition.y - 25, 100, 20};
            GuiSlider(targetSliderBarPos, "Target X (Left) :", "(Right)", &targetCenter.x, 0, winWitdh);
            targetSliderBarPos.y -= 25;
            GuiSlider(targetSliderBarPos, "Target Y (Up) :", "(Down)", &targetCenter.y, 0, winHeight);
        }

        Rectangle noiseSliderBarPos = {500, winHeight - 20, 100, 20};
        GuiSlider(noiseSliderBarPos, "Dist. Noise Ratio (Min) :", "(Max)", &noiseRatio, 0, 100);

        // Left LandMark
        float leftTriXOffset = -200;
        float leftTriYOffset = 700;
        float leftTriCenterX = winWitdh / 2 + leftTriXOffset;
        float leftTriCenterY = leftTriYOffset;
        Vector2 leftTriCenterV = {leftTriCenterX, leftTriCenterY};
        Vector2 leftMidPoint = Vector2Scale(Vector2Add(leftTriCenterV, targetCenter), 0.5f);
        float leftDistance = Vector2Distance(leftTriCenterV, targetCenter);
        sprintf(strBufLeft, "%4.1f mm", leftDistance);

        // Right LandMark
        float rightTriXOffset = 200;
        float rightTriYOffset = 700;
        float rightTriCenterX = winWitdh / 2 + rightTriXOffset;
        float rightTriCenterY = rightTriYOffset;
        Vector2 rightTriCenterV = {rightTriCenterX, rightTriCenterY};

        Vector2 rightMidPoint = Vector2Scale(Vector2Add(rightTriCenterV, targetCenter), 0.5f);
        float rightDistance = Vector2Distance(rightTriCenterV, targetCenter);
        sprintf(strBufRight, "%4.1f mm", rightDistance);

        //----------------------------------------------------------------------------------
        // Drawing
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // Setup the back buffer for drawing (clear color and depth buffers)
        ClearBackground(BLACK);

        // Target
        DrawCircleV(targetCenter, 10, RED);

        // Draw LandMark
        DrawEquilateralTriangle(leftTriCenterV, 5, YELLOW);
        DrawEquilateralTriangle(rightTriCenterV, 5, YELLOW);

        // Draw the Distance dot line
        DrawLineV(leftTriCenterV, targetCenter, GRAY);
        DrawLineV(rightTriCenterV, targetCenter, GRAY);

        // Draw the Distance Value
        DrawTextEx(GetFontDefault(), strBufLeft, leftMidPoint, 30, 1.5, WHITE);
        DrawTextEx(GetFontDefault(), strBufRight, rightMidPoint, 30, 1.5, WHITE);

        // end the frame and get ready for the next one  (display frame, poll input, etc...)
        EndDrawing();
    }

    // destroy the window and cleanup the OpenGL context
    CloseWindow();
    return 0;
}
