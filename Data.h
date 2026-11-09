//
// Created by sushi on 07/11/2026.
//

#ifndef SLITHERLINK_DATA_H
#define SLITHERLINK_DATA_H
#include <vector>

enum class EdgeState {Unknown, Line, Cross};

struct EdgeRef {
    bool isHorizontal;
    int r;
    int c;
};

class Data {
};


#endif //SLITHERLINK_DATA_H
