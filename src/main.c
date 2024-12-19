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
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "particle_filter.h"

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
    uint16_t winWitdh = 2400;
    uint16_t winHeight = 1200;
    Vector2 targetCenter = {winWitdh / 2, 0};
    Vector2 simuCenterHistory[100] = {0};

    float targetCenterX = winWitdh / 2;
    float targetCenterY = 0;
    bool mouseFollow = false;
    char strBufLeft[] = "9999.99 cm";
    char strBufRight[] = "9999.99 cm";
    float noiseRatio = 30;
    float fps = 100;
    float history_numbers = 100;

    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

    // Create the window and OpenGL context
    InitWindow(winWitdh, winHeight, "Partical Filter Demo");

    //particle_filter initial
    init(input_particles);

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
        noiseSliderBarPos.y -= 25;

        GuiSlider(noiseSliderBarPos, "FPS (1) :", "(100)", &fps, 0, 100);
        SetTargetFPS((int)floorf(fps));

        noiseSliderBarPos.y -= 25;
        GuiSlider(noiseSliderBarPos, "History (1) :", "(100)", &history_numbers, 0, 100);

        // Left LandMark
        float leftTriXOffset = -200;
        float leftTriYOffset = 600;
        float leftLandMarkCenterX = winWitdh / 2 + leftTriXOffset;
        float leftLandMarkCenterY = leftTriYOffset;
        Vector2 leftLandMarkCenter = {leftLandMarkCenterX, leftLandMarkCenterY};
        Vector2 leftMidPoint = Vector2Scale(Vector2Add(leftLandMarkCenter, targetCenter), 0.5f);
        float leftDistance = Vector2Distance(leftLandMarkCenter, targetCenter);
        sprintf(strBufLeft, "%4.1f mm", leftDistance);
        // Simu Left Distance
        float simuLeftDistance = leftDistance + noiseRatio * (GetRandomValue(-1000, 1000) / 1000.0);

        // Right LandMark
        float rightTriXOffset = 200;
        float rightTriYOffset = 600;
        float rightLandMarkCenterX = winWitdh / 2 + rightTriXOffset;
        float rightLandMarkCenterY = rightTriYOffset;
        Vector2 rightLandMarkCenter = {rightLandMarkCenterX, rightLandMarkCenterY};
        Vector2 rightMidPoint = Vector2Scale(Vector2Add(rightLandMarkCenter, targetCenter), 0.5f);
        float rightDistance = Vector2Distance(rightLandMarkCenter, targetCenter);
        sprintf(strBufRight, "%4.1f mm", rightDistance);
        // Simu Right Distance
        float simuRightDistance = rightDistance + noiseRatio * (GetRandomValue(-1000, 1000) / 1000.0);

        // Caluate Simu Target Form two Distance Only
        double landmarkDistance = rightTriXOffset - leftTriXOffset;
        double simuX = (simuRightDistance * simuRightDistance - simuLeftDistance * simuLeftDistance - landmarkDistance * landmarkDistance) / (2.0 * landmarkDistance);
        double simuY = sqrt(simuLeftDistance * simuLeftDistance - simuX * simuX);

        // Convert Coordinate system
        Vector2 simuCenter = {-simuX + leftLandMarkCenterX, -simuY + leftLandMarkCenterY};

        if (history_numbers > 1)
        {
            for (size_t HistoryCounter = (size_t)history_numbers - 1; HistoryCounter > (0); HistoryCounter--)
            {
                simuCenterHistory[HistoryCounter] = simuCenterHistory[HistoryCounter - 1];
            }
        }

        simuCenterHistory[0] = simuCenter;

        //----------------------------------------------------------------------------------
        // Partical Filter
        //----------------------------------------------------------------------------------
        speed = Vector2Subtract(filter_center, last_filter_Center);
        polSpeed = CartesianToPol(speed);
        predict(output_particles, input_particles, polSpeed, polStd, GetFrameTime());
        update_weights(output_weights, output_particles, simuLeftDistance, simuRightDistance, 1, leftLandMarkCenter, rightLandMarkCenter);
        systematic_resample(output_indexes, output_weights);
        resample_from_index(input_particles, output_particles, output_indexes);
        //result is in input_particles
        last_filter_Center = filter_center;
        filter_center = input_particles[0];
        filter_center_forDisplay = (Vector2){-filter_center.x + leftLandMarkCenterX, -filter_center.y + leftLandMarkCenterY};

        //----------------------------------------------------------------------------------
        // Drawing
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // Setup the back buffer for drawing (clear color and depth buffers)
        ClearBackground(BLACK);

        // Target
        DrawCircleV(targetCenter, 10, GRAY);

        // Draw LandMark
        DrawEquilateralTriangle(leftLandMarkCenter, 5, BLUE);
        DrawEquilateralTriangle(rightLandMarkCenter, 5, PURPLE);

        // Draw the Distance dot line
        DrawLineV(leftLandMarkCenter, targetCenter, DARKBLUE);
        DrawLineV(rightLandMarkCenter, targetCenter, DARKPURPLE);

        // Draw the Distance Value
        DrawTextEx(GetFontDefault(), strBufLeft, leftMidPoint, 30, 1.5, WHITE);
        DrawTextEx(GetFontDefault(), strBufRight, rightMidPoint, 30, 1.5, WHITE);

        // Draw the Simulation Distance Circle
        DrawCircleLinesV(leftLandMarkCenter, simuLeftDistance, BLUE);
        DrawCircleLinesV(rightLandMarkCenter, simuRightDistance, PURPLE);

        // Draw the Simulation Result
        if (history_numbers > 1)
        {
            for (size_t HistoryCounter = (size_t)history_numbers - 1; HistoryCounter > 0; HistoryCounter--)
            {
                DrawCircleV(simuCenterHistory[HistoryCounter], 5, ColorAlpha(RED, 1.0f - HistoryCounter / (history_numbers * 1.0f)));
            }
        }
        else
        {
            DrawCircleV(simuCenterHistory[0], 5, RED);
        }

        // Draw All Partical Filter Result
        DrawCircleV(filter_center_forDisplay, 5, GREEN);
        // end the frame and get ready for the next one  (display frame, poll input, etc...)
        EndDrawing();
    }

    // destroy the window and cleanup the OpenGL context
    CloseWindow();
    return 0;
}
