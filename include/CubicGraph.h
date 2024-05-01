#ifndef MEDTESTER_CUBIC_GRAPH_H_
#define MEDTESTER_CUBIC_GRAPH_H_

#include <vector>
#include <unordered_set>
#include <iostream>


class CubicGraph
{
public:
    enum class EdgeColor
    {
        NONE,
        MATCHING,
        CYCLE,
        STAR_POINT,
        STAR_CENTER
    };

    // Constructors and deconstructor:
    CubicGraph(const std::vector<std::vector<int>>& adjList, bool checkGraph);
    ~CubicGraph();

    // Getters:
    int getVerticesCount();
    std::vector<std::vector<int>> getAdjList();
    std::vector<std::vector<EdgeColor>> getMedColoring();

    // Functions for reading in and printing out the graph:
    bool printGraph(std::ostream& out);

    // Standard utility functions:
    std::vector<int> bfs(int vertex);
    std::vector<int> dfs(int vertex);

    // Functions for checking properties of the graph:
    bool isMedDecomposable();
    bool isBridgeless();

private:
    int mVerticesCount;
    // note: for each u, v, if (u,v) is edge, than adjListIndices[u][v] = x such that adjList[u][x] = v,
    // otherwise adjListIndices[u][v] = -1
    std::vector<std::vector<int>> mAdjList;
    std::vector<std::vector<int>> mAdjListIndices;
    std::vector<std::vector<CubicGraph::EdgeColor>> mMedColoring;
    bool mColoringDone;
    int mBridgesCount;
    bool mHasBridges;

    void dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited);

    void colorEdge(int vertex, int index, EdgeColor color);
    void generateMedColoring(int vertex);
    void medColoringHelper(int index, std::vector<int>& vertices);

    bool checkCycles();
    bool checkDoubleStars();

    void countBridges(bool findOnlyOne);
};

#endif // MEDTESTER_CUBIC_GRAPH_H_
