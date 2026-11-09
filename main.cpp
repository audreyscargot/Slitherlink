#include <cassert>
#include <iostream>
#include "Grid.h"
#include "Solver.h"
#include "raylib.h"
#include "Tests.h"

int main()
{

    InitWindow(800, 800, "Slitherlink");
    SetTargetFPS(60);

    int current = 0;

    Grid grid;
    Grid initial = grid;                             // pour la touche R

    Solver solver(grid);
    bool animate = true;
    std::string message;

    solver.onStep = [&]() {
        if (!animate) return;
        if (WindowShouldClose()) solver.abort = true;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        grid.draw();
        DrawText("Resolution en cours...", 20, 20, 20, DARKGRAY);
        EndDrawing();
        WaitTime(0.5);
    };

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) {               //partie résolution humaine initiale
            message = solver.Advance() ? "Deductions appliquees" : "Contradiction";
        }
        if (IsKeyPressed(KEY_ENTER)) {               //résolution complète
            message = solver.Solve() ? "Solution trouvee" : "Pas de solution";
        }
        if (IsKeyPressed(KEY_R)) {                   //réinitialiser
            grid = initial;
            message = "";
        }

        if (IsKeyPressed(KEY_L)) {
            std::string err;
            auto rows = ReadPuzzleFile("puzzles/loop_03.txt", &err);
            if (rows.empty()) {
                message = err;
            } else if (LoadPuzzle(grid, rows)) {
                initial = grid;
                message = "Charge : " + std::to_string(grid.getWidth() - 1) + " x "
                        + std::to_string(grid.getHeight() - 1) + " cases";
            } else {
                message = "Grille mal formee";
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        grid.draw();
        DrawText("ESPACE: deductions | ENTREE: resoudre | R: reset",
                 20, 50, 14, DARKGRAY);
        DrawText(message.c_str(), 20, 75, 20, DARKBLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
