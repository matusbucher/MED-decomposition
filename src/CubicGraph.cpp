#include "CubicGraph.h"

#include "typedefs.h"

#include <algorithm>
#include <queue>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <iostream>


const std::vector<std::vector<MEDTester::EdgeType>> VERTEX_COLORINGS = {
    {MEDTester::EdgeType::MATCHING, MEDTester::EdgeType::CYCLE,MEDTester::EdgeType::CYCLE},
    {MEDTester::EdgeType::STAR_LEAF, MEDTester::EdgeType::CYCLE, MEDTester::EdgeType::CYCLE},
    {MEDTester::EdgeType::STAR_CENTER, MEDTester::EdgeType::STAR_LEAF, MEDTester::EdgeType::STAR_LEAF}
};


MEDTester::CubicGraph::CubicGraph(const MEDTester::Matrix& adjList, bool checkGraph = false)
{
    mVerticesCount = adjList.size();
    mAdjList = MEDTester::Matrix(mVerticesCount, MEDTester::MatrixLine(3));
    mAdjListIndices = MEDTester::Matrix(mVerticesCount, MEDTester::MatrixLine(mVerticesCount, -1));

    for (int u = 0; u < mVerticesCount; ++u) {
        for (int i = 0; i < 3; ++i) {
            int v = adjList[u][i];
            mAdjList[u][i] = v;
            mAdjListIndices[u][v] = i;
        }
    }

    mDecomposition = MEDTester::Decomposition();
    mColoringDone = false;
    mBridgesCount = -1;
    mHasBridges = false;
}

MEDTester::CubicGraph::~CubicGraph() {}


int MEDTester::CubicGraph::getVerticesCount() const
{
    return mVerticesCount;
}

MEDTester::Matrix MEDTester::CubicGraph::getAdjList() const
{
    return mAdjList;
}

MEDTester::Decomposition MEDTester::CubicGraph::getDecomposition()
{
    if (mDecomposition.empty()) {
        MEDTester::CubicGraph::generateDecomposition(0);
    }
    return mDecomposition;
}


bool MEDTester::CubicGraph::printGraph(std::ostream& out) const
{
    if (!out.good()) {
        return false;
    }
    out << "Printing graph as adjacency list:\n";
    for (int i = 0; i < mVerticesCount; i++) {
        out << i << ":";
        for (int j = 0; j < 3; j++) {
            out << " " << mAdjList[i][j];
        }
        out << "\n";
    }
    return true;
}


std::vector<int> MEDTester::CubicGraph::bfs(int vertex) const
{
    std::vector<int> returnVector;
    std::unordered_set<int> visited;
    std::queue<int> queue;

    queue.push(vertex);

    while (!queue.empty()) {
        if (queue.front() < 0 || visited.find(queue.front()) != visited.end()) {
            queue.pop();
            continue;
        }

        returnVector.push_back(queue.front());

        for (int i = 0; i < 3; i++) {
            queue.push(mAdjList[queue.front()][i]);
        }

        visited.insert(queue.front());
        queue.pop();
    }

    return returnVector;
}

std::vector<int> MEDTester::CubicGraph::dfs(int vertex) const
{
    std::vector<int> returnVector;
    std::unordered_set<int> visited;
    MEDTester::CubicGraph::dfsHelper(vertex, returnVector, visited);

    return returnVector;
}

void MEDTester::CubicGraph::dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited) const
{
    if (vertex < 0 || visited.find(vertex) != visited.end()) return;

    vec.push_back(vertex);
    visited.insert(vertex);

    for (int i = 0; i < 3; i++) {
        MEDTester::CubicGraph::dfsHelper(mAdjList[vertex][i], vec, visited);
    }
}


bool MEDTester::CubicGraph::isDecomposable() {
    if (mDecomposition.empty()) {
        MEDTester::CubicGraph::generateDecomposition(0);
    }
    if (mDecomposition[0][0] == MEDTester::EdgeType::NONE) return false;
    return true;
}

bool MEDTester::CubicGraph::isBridgeless() {
    if (mBridgesCount == -1) MEDTester::CubicGraph::countBridges(true);
    return mHasBridges;
}


void MEDTester::CubicGraph::colorEdge(int vertex, unsigned int index, MEDTester::EdgeType color)
{
    if (mColoringDone) return;
    mDecomposition[vertex][index] = color;
    mDecomposition[mAdjList[vertex][index]][mAdjListIndices[mAdjList[vertex][index]][vertex]] = color;
}

void MEDTester::CubicGraph::generateDecomposition(int vertex = 0)
{
    mDecomposition = MEDTester::Decomposition(mVerticesCount, std::vector<MEDTester::EdgeType>(3, MEDTester::EdgeType::NONE));
    mColoringDone = false;
    std::vector<int> vertices = MEDTester::CubicGraph::dfs(vertex);

    for (const std::vector<MEDTester::EdgeType>& colors : VERTEX_COLORINGS) {
        for (int r = 0; r < 3 && !mColoringDone; r++) {
            for (int i = 0; i < 3; ++i) {
                MEDTester::CubicGraph::colorEdge(0, i, colors[(i+r) % 3]);
            }
            MEDTester::CubicGraph::decompositionHelper(1, vertices);
        }
    }

    if (!mColoringDone) {
        for (int i = 0; i < 3; ++i) {
            MEDTester::CubicGraph::colorEdge(0, i, MEDTester::EdgeType::NONE);
        }
    }
}

void MEDTester::CubicGraph::decompositionHelper(unsigned int index, std::vector<int>& vertices)
{
    if (mColoringDone) return;
    
    if (index >= vertices.size()) {
        mColoringDone = MEDTester::CubicGraph::checkCycles() && MEDTester::CubicGraph::checkDoubleStars();
        return;
    }

    int v = vertices[index];
    std::vector<int> uncolored;
    std::vector<int> edgeColorsCount(5, 0);

    for (int i = 0; i < 3; i++) {
        if (mDecomposition[v][i] == MEDTester::EdgeType::NONE) {
            uncolored.push_back(i);
        }
        ++edgeColorsCount[(int) mDecomposition[v][i]];
    }

    /* If at least one edge is double-star leaf edge, we check if the coloring of adjacent edges
     * is still "correct". If it is, than we determine on which "side" of double-star leaf edge
     * is this vertex.  */
    bool starCenterVertex = false;
    bool starLeafVertex = false;
    if (edgeColorsCount[3]) {
        std::vector<int> leafEdges;
        for (int i = 0; i < 3; ++i) {
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_LEAF) {
                leafEdges.push_back(i);
            }
        }

        /* If another edge is cycle edge, than the double-star leaf edge should be adjacent to
         * double-star center edge. */
        if (edgeColorsCount[2]) {
            if (edgeColorsCount[0] + edgeColorsCount[2] + edgeColorsCount[3] != 3 || edgeColorsCount[3] == 2) return;
            for (int i : leafEdges) {
                bool checked = false;
                for (int j = 0; j < 3; ++j) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_CENTER) {
                        checked = true;
                        break;
                    }
                }
                if (!checked) return;
                starLeafVertex = true;
            }
        }

        /* If another edge is double-star center edge, than the double-star leaf edge should be
         * adjacent to double-star center edge. */
        else if (edgeColorsCount[4]) {
            if (edgeColorsCount[0] + edgeColorsCount[3] + edgeColorsCount[4] != 3 || edgeColorsCount[4] == 2) return;
            for (int i : leafEdges) {
                bool checked = false;
                for (int j = 0; j < 3; ++j) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::CYCLE) {
                        checked = true;
                        break;
                    }
                }
                if (!checked) return;
                starCenterVertex = true;
            }
        }

        else if (edgeColorsCount[0] + edgeColorsCount[3] != 3) return;

        else {
            for (int i : leafEdges) {
                for (int j = 0; j < 3; ++j) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::CYCLE) {
                        if (starLeafVertex) return;
                        starCenterVertex = true;
                        break;
                    }
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_CENTER) {
                        if (starCenterVertex) return;
                        starLeafVertex = true;
                        break;
                    }
                }
            }
        }
    }

    /* We also check if there is not only one double-star center edge (with no leaf edges). */
    else if (edgeColorsCount[4]) {
        if (edgeColorsCount[0] != 2) return;
        starCenterVertex = true;
    }


    /* If starLeafVertex is set to true, than the uncolored edges must be cycle edges. */
    if (starLeafVertex) {
        for (int i : uncolored) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::CYCLE);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If starCenterVertex is set to true, than one edge must be double-star center edge and
     * the other two double-star leaf edges. */
    else if (starCenterVertex) {
        if (edgeColorsCount[4]) {
            for (int i : uncolored) {
                MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::STAR_LEAF);
            }
            MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
        }
        else if (edgeColorsCount[3] == 2) {
            MEDTester::CubicGraph::colorEdge(v, uncolored[0], MEDTester::EdgeType::STAR_CENTER);
        }
        else if (edgeColorsCount[0] == 2) {
            MEDTester::CubicGraph::colorEdge(v, uncolored[0], MEDTester::EdgeType::STAR_CENTER);
            MEDTester::CubicGraph::colorEdge(v, uncolored[1], MEDTester::EdgeType::STAR_LEAF);

            MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
            if (mColoringDone) return;

            MEDTester::CubicGraph::colorEdge(v, uncolored[0], MEDTester::EdgeType::STAR_LEAF);
            MEDTester::CubicGraph::colorEdge(v, uncolored[1], MEDTester::EdgeType::STAR_CENTER);
            
            MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
        }
        else return;
    }

    /* If one edge is a matching edge, then other two edges must be in a cycle. */
    else if (edgeColorsCount[1] == 1 && edgeColorsCount[0] + edgeColorsCount[2] == 2) {
        for (int i : uncolored) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::CYCLE);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If two edgeColorsCount are in a cycle, then the remaining one must be either a matching edge or
     * a double-star leaf edge (and also is uncolored, because of previous checks). */
    else if (edgeColorsCount[2] == 2 && edgeColorsCount[0]) {
        MEDTester::CubicGraph::colorEdge(v, uncolored[0], MEDTester::EdgeType::MATCHING);

        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
        if (mColoringDone) return;

        MEDTester::CubicGraph::colorEdge(v, uncolored[0], MEDTester::EdgeType::STAR_LEAF);

        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle and two edges are not colored, then one of them must be in a cycle and
     * the other must be either a matching edge or a double-star leaf edge. */
    else if (edgeColorsCount[2] == 1 && edgeColorsCount[0] == 2) {
        MEDTester::EdgeType option[] = {MEDTester::EdgeType::MATCHING, MEDTester::EdgeType::STAR_LEAF};
        for (int i = 0; i < 2; i++) {
            MEDTester::CubicGraph::colorEdge(v, uncolored.at(i), MEDTester::EdgeType::CYCLE);
            for (int j = 0; j < 2; j++) {
                MEDTester::CubicGraph::colorEdge(v, uncolored.at((i+1)%2), option[j]);
                MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
            }
        }
    }

    if (!mColoringDone) {
        for (int i : uncolored) MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::NONE);
    }
}


bool MEDTester::CubicGraph::checkCycles() const
{
    std::vector<bool> checked(mVerticesCount, false);

    for (int v = 0; v < mVerticesCount; v++) {
        if (!checked[v]) {
            checked[v] = true;
            bool isCycle = false;
            int next = -1;
            for (int j = 0; j < 3; j++) {
                if (mDecomposition[v][j] == MEDTester::EdgeType::CYCLE) {
                    isCycle = true;
                    next = mAdjList[v][j];
                    break;
                }
            }
            if (isCycle) {
                int length = 1;
                int prev = v;
                int maxIter = mVerticesCount;
                while (next != v && maxIter--) {
                    checked[next] = true;
                    length++;
                    for (int j = 0; j < 3; j++) {
                        if (mDecomposition[next][j] == MEDTester::EdgeType::CYCLE && mAdjList[next][j] != prev) {
                            prev = next;
                            next = mAdjList[next][j];
                            break;
                        }
                    }
                }
                if (length % 2 == 1 || maxIter == 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool MEDTester::CubicGraph::checkDoubleStars() const
{
    std::unordered_set<int> starVertices;

    for (int v = 0; v < mVerticesCount; v++) {
        bool isStarVertex = false;
        for (int i = 0; i < 3; i++) {
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_CENTER) {
                isStarVertex = false;
                break;
            }
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_LEAF) {
                isStarVertex = true;
            }
        }
        if (isStarVertex) {
            for (int i = 0; i < 3; i++) {
                if (starVertices.find(mAdjList[v][i]) != starVertices.end()) {
                    return false;
                }
            }
            starVertices.insert(v);
        }
    }
    return true;
}


void MEDTester::CubicGraph::countBridges(bool foundOnlyOne = false)
{
    mBridgesCount = 0;
    for (int i = 0; i < mVerticesCount-1; i++) {
        for (int j = i+1; j < mVerticesCount; j++) {
            if (mAdjListIndices[i][j] != -1) {
                mAdjList[i][mAdjListIndices[i][j]] = -1;
                int n = MEDTester::CubicGraph::dfs(i).size();
                mAdjList[i][mAdjListIndices[i][j]] = j;
                if (n != mVerticesCount) {
                    mBridgesCount++;
                    if (foundOnlyOne) {
                        mHasBridges = true;
                        return;
                    }
                }
            }
        }
    }

    if (mBridgesCount) {
        mHasBridges = true;
    }
}
