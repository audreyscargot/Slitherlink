//
// Created by sushi on 08/11/2026.
//

#include "Tests.h"
#include <fstream>
#include <iterator>
#include <sstream>
#include "Solver.h"

bool LoadPuzzle(Grid& g, const std::vector<std::string>& rows) {
    if (rows.empty()) return false;
    const int cellRows = (int)rows.size();
    const int cellCols = (int)rows[0].size();
    for (const std::string& line : rows)
        if ((int)line.size() != cellCols) return false;

    g.init(cellCols + 1, cellRows + 1);          // points = cases + 1
    for (int r = 0; r < cellRows; r++)
        for (int c = 0; c < cellCols; c++) {
            char ch = rows[r][c];
            if (ch >= '0' && ch <= '9') g.GetClue(r, c) = ch - '0';
        }
    return true;
}

static int CountLines(Grid& g) {
    int n = 0;
    for (int r = 0; r < g.getHeight(); r++)
        for (int c = 0; c < g.getWidth() - 1; c++)
            if (g.GetREdge(r, c) == EdgeState::Line) n++;
    for (int r = 0; r < g.getHeight() - 1; r++)
        for (int c = 0; c < g.getWidth(); c++)
            if (g.GetCEdge(r, c) == EdgeState::Line) n++;
    return n;
}

std::vector<std::string> ReadPuzzleFile(const std::string& path, std::string* error) {
    std::vector<std::string> rows;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        if (error) *error = "Fichier introuvable : " + path;
        return rows;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // BOM UTF-8 éventuel
    if (content.rfind("\xEF\xBB\xBF", 0) == 0) content.erase(0, 3);

    // "…" (UTF-8) puis "…" (Windows-1252, un seul octet) -> "..."
    size_t pos;
    while ((pos = content.find("\xE2\x80\xA6")) != std::string::npos) content.replace(pos, 3, "...");
    while ((pos = content.find('\x85')) != std::string::npos) content.replace(pos, 1, "...");

    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    while (std::getline(stream, line)) {
        lineNumber++;

        std::string clean;
        for (char ch : line)
            if (ch != ' ' && ch != '\t' && ch != '\r') clean += ch;
        if (clean.empty() || clean[0] == '#') continue;

        for (char ch : clean) {
            bool valid = (ch >= '0' && ch <= '9') || ch == '.';
            if (!valid) {
                if (error) *error = "Ligne " + std::to_string(lineNumber) + " : caractere invalide";
                return {};
            }
        }
        rows.push_back(clean);
    }
    if (rows.empty() && error) *error = "Fichier vide";
    return rows;
}
