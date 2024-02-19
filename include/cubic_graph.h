#include <vector>
#include <unordered_set>
#include <iostream>

#ifndef CUBIC_GRAPH_H
#define CUBIC_GRAPH_H

enum class EdgeColor {
    NONE,
    MATCHING,
    CYCLE,
    STAR_POINT,
    STAR_CENTER
};

class CubicGraph {
private:
    int verticesCount;
    // for each u, v, if (u,v) is edge, than adjListIndices[u][v] = x such that adjList[u][x] = v,
    // otherwise adjListIndices[u][v] = -1
    int** adjList;
    int** adjListIndices;
    EdgeColor** medColoring;
    bool coloringDone;
    int bridgesCount;

    void dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited);

    void generateMedColoring(int vertex);

    void medColoringHelper(int index, std::vector<int>& vertices);

    void colorEdge(int vertex, int index, EdgeColor color);

    bool checkCycles();

    bool checkDoubleStars();

    void countBridges();

public:
    CubicGraph(int n);

    ~CubicGraph();

    int getVerticesCount();

    int** getAdjList();

    void readGraph(std::istream& in);

    void printGraph(std::ostream& out);

    std::vector<int> bfs(int vertex);

    std::vector<int> dfs(int vertex);

    EdgeColor** getMedColoring();

    bool isMedDecomposable();

    bool isBridgeless();
};

#endif