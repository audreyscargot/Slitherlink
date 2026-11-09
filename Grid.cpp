//
// Created by sushi on 07/11/2026.
//

#include "Grid.h"
#include <iostream>
#include "raylib.h"
int spacing = 60;
int ogX = 200;
int ogY = 200;
float radius = 10;
int xSize = 11;
int ySize = 10;

//INITIALISATION------------------------
void Grid::init(int _width, int _height) {
    width = _width;
    height = _height;

    clues.assign((width - 1) * (height - 1), -1); //créer tableau rempli de -1 (si valeur tableau = -1 -> pas d'indices)
    rEdges.assign((width - 1) * height, EdgeState::Unknown);
    cEdges.assign(width * (height - 1), EdgeState::Unknown);
}

//GETTER--------------------------------
int Grid::getWidth() const {
    return width;
}

int Grid::getHeight() const {
    return height;
}

EdgeState& Grid::GetREdge(int r, int c) {
    return rEdges[r * (getWidth() - 1) +c];
}

EdgeState& Grid::GetCEdge(int r, int c) {
    return cEdges[r * getWidth() +c];
}

int & Grid::GetClue(int r, int c) {
    return clues[r * (getWidth() - 1) +c];
}

EdgeState& Grid::GetEdgeRef(const EdgeRef& e) {
    return e.isHorizontal ? GetREdge(e.r, e.c) : GetCEdge(e.r, e.c);
}

std::vector<EdgeRef> Grid::GetCellEdges(int r, int c) { //récupérer les arrêtes autour d'une case
    return {
        {true, r,c}, //haut
        {true, r+1,c}, //bas
        {false,r,c}, //gauche
        {false,r,c+1}, //droite
    };
}

std::vector<EdgeRef> Grid::GetPointEdges(int r, int c) { //récupérer les arrêtes autour d'un point
    std::vector<EdgeRef> list;
    if (c > 0) list.push_back({true,r,c-1}); //gauche
    if (c < width - 1) list.push_back({true,r,c}); //droite
    if (r > 0) list.push_back({false, r-1,c}); // haut
    if (r < height - 1) list.push_back({false, r,c}); //bas
    return list;
}

//SET EDGE WITH CONDITIONS--------------
bool Grid::SetEdgeRef(EdgeRef& e, EdgeState s) {
    EdgeState currentState = GetEdgeRef(e);
    if (currentState == s) return true; //pas besoin de changer
    if (currentState != EdgeState::Unknown) return false; // ne réécris pas sur une arrête déjà "validée"

    GetEdgeRef(e) = s;
    //dire à la réflexion de continuer
    return true;

}

//COUNT---------------------------------
void Grid::countEdges(bool isCellCount, int &nbLine, int &nbCross, int &nbUnknown, int r, int c) {
    nbLine = nbCross = nbUnknown = 0;
    const auto edges = isCellCount ? GetCellEdges(r,c) : GetPointEdges(r,c);
    for (const EdgeRef& e : edges) {
        switch (GetEdgeRef(e)) {
            case EdgeState::Unknown: nbUnknown++; break;
            case EdgeState::Line: nbLine++; break;
            case EdgeState::Cross: nbCross++; break;
        }
    }
}

//DRAW FUNCTION-------------------------
void Grid::draw() const {

    //lignes
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width - 1; j++) {
            EdgeState temp = rEdges[i * (width - 1) + j];
            int x1 = ogX + j * spacing;
            int x2 = ogX + (j+1) * spacing;
            int y = ogY + i * spacing;
            switch (temp) {
                case EdgeState::Unknown: DrawLine(x1, y, x2, y, WHITE); break;
                case EdgeState::Line : DrawLine(x1, y, x2, y, BLACK); break;
                case EdgeState::Cross : DrawText(TextFormat("X"), (x1+x2)/2-xSize, y-ySize, 15, RED); break;
            }
        }
    }

    //colonnes
    for (int i = 0; i < height-1; i++) {
        for (int j = 0; j < width; j++) {
            EdgeState temp = cEdges[i * width + j];
            int y1 = ogY + i * spacing;
            int y2 = ogY + (i+1) * spacing;
            int x = ogX + j * spacing;
            switch (temp) {
                case EdgeState::Unknown: DrawLine(x, y1, x, y2, WHITE); break;
                case EdgeState::Line : DrawLine(x, y1, x, y2, BLACK); break;
                case EdgeState::Cross : DrawText(TextFormat("X"), x-xSize, (y1+y2)/2-ySize, 25, RED); break;
            }
        }
    }

    //int
    for (int i = 0; i < height-1; i++) {
        for (int j = 0; j < width - 1; j++) {
            int c = clues[i * (width - 1) + j];
            if (c >= 0) { //condition c >= 0 car si c = -1 -> pas d'indices
                DrawText(TextFormat("%d", c), ogX + j * spacing + spacing/2 - xSize, ogY + i * spacing+spacing/2 - ySize,25, BLACK);
            }
        }
    }

    //points
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            DrawCircle(ogX + j * spacing, ogY + i * spacing, radius, BLACK);
        }
    }
}

//---------------------DEBUG DEMANDES IA-------------------------------------

// Grid.cpp
void Grid::print() const {
    for (int r = 0; r < height; r++) {
        // ligne des points et arêtes horizontales
        for (int c = 0; c < width; c++) {
            std::cout << "+";
            if (c < width - 1) {
                EdgeState s = rEdges[r * (width - 1) + c];
                std::cout << (s == EdgeState::Line ? "---" : s == EdgeState::Cross ? " x " : "   ");
            }
        }
        std::cout << "\n";
        // ligne des arêtes verticales et indices
        if (r < height - 1) {
            for (int c = 0; c < width; c++) {
                EdgeState s = cEdges[r * width + c];
                std::cout << (s == EdgeState::Line ? "|" : s == EdgeState::Cross ? "x" : " ");
                if (c < width - 1) {
                    int n = clues[r * (width - 1) + c];
                    std::cout << " " << (n >= 0 ? char('0' + n) : ' ') << " ";
                }
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

