#ifndef CUBIC_GRAPH_H
#define CUBIC_GRAPH_H

#include <vector>
#include <unordered_set>
#include <iostream>

class cubicGraph {

private:
    int numVertices;
    // for each u, v, if (u,v) is edge, than adjListIndices[u][v] = x such that adjList[u][x] = v,
    // otherwise adjListIndices[u][v] = -1
    int** adjList;
    int** adjListIndices;
    char** medColoring;
    bool coloringDone;
    int hasNoBridge;

    void dfsHelper(int, std::vector<int>&, std::unordered_set<int>&);
    void generateMedColoring(int);
    void medColoringHelper(int, std::vector<int>&);
    void colorEdge(int, int, char);
    bool checkCycles();
    bool checkDoubleStars();

public:
    cubicGraph(int);
    ~cubicGraph();

    void readGraph(std::istream&);
    void printGraph(std::ostream&);

    std::vector<int> bfs(int);
    std::vector<int> dfs(int);

    char** getMedColoring();
    bool isMedDecomposable();

    bool isBridgeless();
};

#endif