#ifndef MEDTESTER_CUBIC_GRAPH_H_
#define MEDTESTER_CUBIC_GRAPH_H_

#include "typedefs.h"

#include <vector>
#include <unordered_set>
#include <iostream>


namespace MEDTester
{

class CubicGraph
{
public:
    // Constructors and deconstructor:
    CubicGraph(const MEDTester::Matrix& adjList, bool checkGraph);
    ~CubicGraph();

    // Getters:
    int getVerticesCount();
    MEDTester::Matrix getAdjList();
    MEDTester::Decomposition getDecomposition();

    // Functions for reading in and printing out the graph:
    bool printGraph(std::ostream& out);

    // Standard utility functions:
    std::vector<int> bfs(int vertex);
    std::vector<int> dfs(int vertex);

    // Functions for checking properties of the graph:
    bool isDecomposable();
    bool isBridgeless();

private:
    int mVerticesCount;
    MEDTester::Matrix mAdjList;
    MEDTester::Matrix mAdjListIndices;
    MEDTester::Decomposition mDecomposition;
    bool mColoringDone;
    int mBridgesCount;
    bool mHasBridges;

    void dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited);

    void colorEdge(int vertex, int index, MEDTester::EdgeType color);
    void generateDecomposition(int vertex);
    void decompositionHelper(int index, std::vector<int>& vertices);

    bool checkCycles();
    bool checkDoubleStars();

    void countBridges(bool findOnlyOne);
};

} // namespace MEDTester

#endif // MEDTESTER_CUBIC_GRAPH_H_
