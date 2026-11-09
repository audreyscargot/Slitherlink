//
// Created by sushi on 07/11/2026.
//

#ifndef SLITHERLINK_GRID_H
#define SLITHERLINK_GRID_H
#include <vector>

#include "Data.h"

class Grid {
public :
    void init(int _width, int _height);
    void draw() const;

    int getWidth() const;
    int getHeight() const;

    EdgeState& GetREdge(int r, int c);
    EdgeState& GetCEdge(int r, int c);
    int& GetClue(int r, int c);

    EdgeState& GetEdgeRef(const EdgeRef& e);
    bool SetEdgeRef(EdgeRef& e, EdgeState s);

    std::vector<EdgeRef> GetCellEdges(int r, int c);
    std::vector<EdgeRef> GetPointEdges(int r, int c);

    void countEdges(bool isCellCount,int &nbLine, int &nbCross, int &nbUnknown, int r, int c);

    void print() const;

private :
    int width = 0;
    int height = 0;
    std::vector<int> clues;
    std::vector<EdgeState> rEdges;
    std::vector<EdgeState> cEdges;
};


#endif //SLITHERLINK_GRID_H
