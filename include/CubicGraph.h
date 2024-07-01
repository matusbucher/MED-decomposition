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
    CubicGraph(const MEDTester::Matrix& adjList);
    ~CubicGraph();

    // Getters:
    int getVerticesCount() const;
    int getEdgesCount() const;
    MEDTester::Matrix getAdjList() const;
    MEDTester::Matrix getAdjListIndices() const;
    MEDTester::Decomposition getDecomposition();
    int getDecompositionsCount();
    int getBridgesCount();

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
    int mEdgesCount;
    MEDTester::Matrix mAdjList;
    MEDTester::Matrix mAdjListIndices;
    MEDTester::Decomposition mDecomposition;
    bool mDecompositionDone;
    int mDecompositionsCount;
    int mBridgesCount;
    bool mAllBridgesFound;

    void dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited) const;

    void assignEdge(int vertex, unsigned int index, MEDTester::EdgeType type);
    void generateDecomposition(int vertex, bool counting);
    void decompositionHelper(unsigned int index, std::vector<int>& vertices, bool counting);

    bool checkCycles() const;
    bool checkDoubleStars() const;

    void countBridges(bool findOnlyOne);
    void countBridgesHelper(int vertex, int parent, std::vector<bool>& visited, std::vector<int>& disc, std::vector<int>& low, int *time, bool findOnlyOne);
};

} // namespace MEDTester

#endif // MEDTESTER_CUBIC_GRAPH_H_
