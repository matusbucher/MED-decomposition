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
    int getVerticesCount() const;
    MEDTester::Matrix getAdjList() const;
    MEDTester::Decomposition getDecomposition();

    // Functions for reading in and printing out the graph:
    bool printGraph(std::ostream& out) const;

    // Standard utility functions:
    std::vector<int> bfs(int vertex) const;
    std::vector<int> dfs(int vertex) const;

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

    void dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited) const;

    void colorEdge(int vertex, unsigned int index, MEDTester::EdgeType color);
    void generateDecomposition(int vertex);
    void decompositionHelper(unsigned int index, std::vector<int>& vertices);

    bool checkCycles() const;
    bool checkDoubleStars() const;

    void countBridges(bool findOnlyOne);
};

} // namespace MEDTester

#endif // MEDTESTER_CUBIC_GRAPH_H_
