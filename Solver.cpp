//
// Created by sushi on 07/11/2026.
//

#include "Solver.h"

#include "raylib.h"


bool Solver::Solve() {
    abort = false;
    steps = 0;
    Grid start = grid;
    if (Advance() && Backtrack()) return true;
    grid = start;
    return false;
}

//check si une run est valide
bool Solver::CheckIsSafe() {
    int lines, crosses, unknowns;
    //pour les cases
    bool ok = forEachCell([&](int i, int j) {
        int clue = grid.GetClue(i, j);
        if (clue < 0) return true;

        grid.countEdges(true, lines, crosses, unknowns, i, j);
        if (lines > clue) return false; //trop de line par rapport au nombre demandé
        if (lines + unknowns < clue) return false; //ça veut dire qu'une line a été crossed sans devoir l'être
        return true;
    });
    if (!ok) return false;

    //pour les points
    ok = forEachPoint([&](int i, int j) {
        grid.countEdges(false, lines, crosses, unknowns, i, j);
        if (lines > 2) return false; //un point ne peut avoir que 2 arrêtes max sinon un chemin est coupé
        if (lines == 1 && unknowns == 0) return false; //tous les chemins sont crossed alors c'est impasse donc incorrect
        return true;
    });
    if (!ok) return false;
    return true;
}

//check si le puzzle est solved
bool Solver::CheckIsSolved() {
    if (!CheckIsSafe()) return false;

    //début check si on a bien une loop d'edge
    int h = grid.getHeight(), w = grid.getWidth();
    std::vector<bool> visitedPoint(h*w, false);

    int totalLoop = 0; //total des lignes dans la boucle
    int start = -1;
    //compte les points avec 2 arrêtes
    forEachPoint([&](int i, int j) {
        int l, c, u;
        grid.countEdges(false, l, c, u, i, j);
        if (l == 2) {
            totalLoop++;
            if (start < 0) start = i * w + j; //trouve un point de départ pour la boucle
        }
        return true;
    });
    if (totalLoop == 0) return false;

    //parcours depuis un point start
    std::vector<int> loop = {start};
    visitedPoint[start] = true;
    int reached = 0;
    while (!loop.empty()) {
        int point = loop.back();
        reached++;
        loop.pop_back();
        int row = point / w; //récupérer Y de la row du point
        int col = point % w; //récupérer X de la col du point
        for (const EdgeRef& edge : grid.GetPointEdges(row, col)) {
            if (grid.GetEdgeRef(edge) != EdgeState::Line) continue;
            int nextr = row, nextc = col; //autre côté de l'arrête
            if (edge.isHorizontal) nextc = (edge.c == col) ? col+1 : col-1;
            else nextr = (edge.r == row) ? row+1 : row-1;
            int nextPoint = nextr * w + nextc; //position du point suivant
            if (!visitedPoint[nextPoint]) {
                visitedPoint[nextPoint] = true;
                loop.push_back(nextPoint);
            }
        }
    }
    return reached == totalLoop;
}

//fonction implémentant les "rélfexions humaines"
bool Solver::Think() {
    bool changed = false;

    // Func pour mettre tous les unknows a l'état s
    auto fill = [&](std::vector<EdgeRef>& edges, EdgeState s) {
        for (EdgeRef& e : edges) {
            if (grid.GetEdgeRef(e) == EdgeState::Unknown) {
                grid.SetEdgeRef(e, s);
                changed = true;
            }
        }
    };

    //REGLES DES CASES-------------------
    forEachCell([&](int i, int j) {
        int clue = grid.GetClue(i, j);
        if (clue < 0) return true;

        int lines, crosses, unknowns;
        grid.countEdges(true, lines, crosses, unknowns, i, j);
        if (unknowns == 0) return true;

        auto edges = grid.GetCellEdges(i,j);

        //R1 : si la case est pleine
        if (lines == clue) fill(edges, EdgeState::Cross); //toutes les cases restantes sont crossed

        //R2 : si le nombre de line et unknown donne clue
        else if (lines + unknowns == clue) fill(edges, EdgeState::Line);

        return true;
    });

    //REGLES DES POINTS------------------
    forEachPoint([&](int i, int j) {
        int lines, crosses, unknowns;
        grid.countEdges(false, lines, crosses, unknowns, i, j);
        if (unknowns == 0) return true;

        auto edges = grid.GetPointEdges(i,j);

        //R3 : si un point a déjà 2 lignes -> on barre le reste
        if (lines == 2) fill(edges, EdgeState::Cross);

        //R4 : si un point a un trait et que le reste est barré -> une seule possibilité
        else if (lines == 1 && unknowns == 1) fill(edges, EdgeState::Line);

        //R5 : si un point a tout de cross sauf un, le dernier est forcément un cross sinon on a une ligne seule
        else if (lines == 0 && unknowns == 1) fill(edges, EdgeState::Cross);

        return true;
    });

    return changed;
}

//propagation
bool Solver::Advance() {
    if (!CheckIsSafe()) return false;
    while (Think()) {
        Step();
        if (!CheckIsSafe()) return false;
    }
    return true;
}

bool Solver::FindUnknown(EdgeRef& edgeRef) {

    //priorité 1 : si une case a déjà une ligne et des unknowns
    for (int i = 0; i < grid.getHeight(); i++) {
        for (int j = 0; j < grid.getWidth(); j++) {
            int l,c,u;
            grid.countEdges(false, l, c, u, i, j);
            if (l == 1 && u >= 1) {
                for (EdgeRef& edge : grid.GetPointEdges(i,j)) {
                    if (grid.GetEdgeRef(edge) == EdgeState::Unknown) {
                        edgeRef = edge;
                        return true;
                    }
                }
            }
        }
    }

    //priorité 2 : si une case n'a pas son nombre de clue complet
    for (int i = 0; i < grid.getHeight()-1; i++) {
        for (int j = 0; j < grid.getWidth()-1; j++) {
            int l,c,u;
            grid.countEdges(true, l, c, u, i, j);
            if (grid.GetClue(i,j) >= 0 && u >= 1) {
                for (EdgeRef& edge : grid.GetCellEdges(i,j)) {
                    if (grid.GetEdgeRef(edge) == EdgeState::Unknown) {
                        edgeRef = edge;
                        return true;
                    }
                }
            }
        }
    }

    //priorité 3 : n'importe laquelle
    //cells horizontales
    for (int i = 0; i < grid.getHeight(); i++) {
        for (int j = 0; j < grid.getWidth()-1; j++) {
            if (grid.GetREdge(i,j) == EdgeState::Unknown) {
                edgeRef = {true, i, j};
                return true;
            }
        }
    }

    //cells verticales
    for (int i = 0; i < grid.getHeight()-1; i++) {
        for (int j = 0; j < grid.getWidth(); j++) {
            if (grid.GetCEdge(i,j) == EdgeState::Unknown) {
                edgeRef = {false, i, j};
                return true;
            }
        }
    }
    return false;
}

bool Solver::Backtrack() {
    if (abort) return false;

    EdgeRef e{};
    if (!FindUnknown(e)) return CheckIsSolved();

    Grid save = grid;
    grid.SetEdgeRef(e, EdgeState::Line);
    Step();
    if (Advance() && Backtrack()) return true;
    grid = save;
    Step();

    grid.SetEdgeRef(e, EdgeState::Cross);
    Step();
    if (Advance() && Backtrack()) return true;
    grid = save;
    Step();
    return false;

}

void Solver::Step() {
    steps++;
    if (onStep) onStep();
}
