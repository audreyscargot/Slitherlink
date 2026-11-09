//
// Created by sushi on 07/11/2026.
//

#ifndef SLITHERLINK_SOLVER_H
#define SLITHERLINK_SOLVER_H
#include <functional>

#include "Grid.h"


class Solver {
    template <typename Func>
bool forEachCell(Func f) const {
        for (int i = 0; i < grid.getHeight()-1; i++)
            for (int j = 0; j < grid.getWidth()-1; j++)
                if (!f(i, j)) return false;
        return true;
    }

    template <typename Func>
bool forEachPoint(Func f) const {
        for (int i = 0; i < grid.getHeight(); i++)
            for (int j = 0; j < grid.getWidth(); j++)
                if (!f(i, j)) return false;
        return true;
    }

    protected :
    Grid& grid;

public:
    Solver (Grid& g) : grid(g) {}

    bool Solve();
    bool CheckIsSafe(); //check si la solution actuelle est possible
    bool CheckIsSolved();
    bool Think(); //fonction qui donnent les règles de réflexion
    bool Advance(); //fonction qui applique les règles et continue si IsSafe
    bool FindUnknown(EdgeRef& edgeRef);

    bool Backtrack();

    std::function<void()> onStep;
    void Step();
    bool abort = false;
    int steps = 0;

};


#endif //SLITHERLINK_SOLVER_H
