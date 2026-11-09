//
// Created by sushi on 08/11/2026.
//

#ifndef SLITHERLINK_TESTS_H
#define SLITHERLINK_TESTS_H
#include <string>
#include <vector>
#include "Grid.h"

struct TestCase {
    std::string name;
    std::vector<std::string> rows;   // une chaîne par ligne de cases
    bool solvable;
    int expectedLines;               // longueur de la boucle, -1 = ne pas vérifier
};

bool LoadPuzzle(Grid& g, const std::vector<std::string>& rows);
std::vector<std::string> ReadPuzzleFile(const std::string& path, std::string* error = nullptr);

#endif
