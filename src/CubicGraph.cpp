#include "CubicGraph.h"

#include "typedefs.h"

#include <algorithm>
#include <queue>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <iostream>


const MEDTester::EdgeType VERTEX_COLORINGS[3][3] = {
    {MEDTester::EdgeType::MATCHING, MEDTester::EdgeType::CYCLE,MEDTester::EdgeType::CYCLE},
    {MEDTester::EdgeType::STAR_CENTER, MEDTester::EdgeType::STAR_POINT, MEDTester::EdgeType::STAR_POINT},
    {MEDTester::EdgeType::STAR_POINT, MEDTester::EdgeType::CYCLE, MEDTester::EdgeType::CYCLE}
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


void MEDTester::CubicGraph::colorEdge(int vertex, int index, MEDTester::EdgeType color)
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

    MEDTester::CubicGraph::decompositionHelper(0, vertices);
}

void MEDTester::CubicGraph::decompositionHelper(int index, std::vector<int>& vertices)
{
    if (mColoringDone) return;
    
    if (index == (int) vertices.size()) {
        mColoringDone = MEDTester::CubicGraph::checkCycles() && MEDTester::CubicGraph::checkDoubleStars();
        return;
    }

    int v = vertices.at(index);
    std::vector<int> uncolored;
    int edges[5];
    memset(edges, 0, sizeof(edges));

    for (int i = 0; i < 3; i++) {
        switch (mDecomposition[v][i]) {
            case MEDTester::EdgeType::NONE:
                edges[0]++;
                uncolored.push_back(i);
                break;
            case MEDTester::EdgeType::MATCHING:
                edges[1]++;
                break;
            case MEDTester::EdgeType::CYCLE:
                edges[2]++;
                break;
            case MEDTester::EdgeType::STAR_POINT:
                edges[3]++;
                break;
            case MEDTester::EdgeType::STAR_CENTER:
                edges[4]++;
                break;
            default:
                std::abort();
                break;
        }
    }

    /* Firstly, if one of edges is double-star point edge and at least one other edge is
     * a cycle egde or matching edge, then we check, if there is no other cycle or matching
     * edge incident to the double-star edge on the other side. */
    if (edges[3] == 1 && edges[1] + edges[2] > 0) {
        for (int i = 0; i < 3; i++) {
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_CENTER) {
                for (int j = 0; j < 3; j++) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::MATCHING || mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::CYCLE) {
                        return;
                    }
                }
            }
        }
    }

    /* No edge of vertex was colored yet -> choose random (but correct) coloring on edges. */
    if (edges[0] == 3) {
        for (const MEDTester::EdgeType* colors : VERTEX_COLORINGS) {
            for (int r = 0; r < 3; r++) {
                for (int i : uncolored) {
                    MEDTester::CubicGraph::colorEdge(v, i, colors[(i+r) % 3]);
                }
                MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
            }
        }
    }

    /* If two edges are in a cycle, then the remaining one must be either a matching edge or
     * a double-star point edge. */
    else if (edges[2] == 2 && edges[0] + edges[1] + edges[3] == 1) {
        bool canSEdge = true;
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::CYCLE || mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::MATCHING) {
                    canSEdge = false;
                }
            }
            if (canSEdge) {
                MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::STAR_POINT);
                MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
            }
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::MATCHING);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If one edge is a matching edge, then other two edges must be in a cycle. */
    else if (edges[1] == 1 && edges[0] + edges[2] == 2) {
        for (int i : uncolored) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::CYCLE);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle, one edge is either a matching or a double-star point edge,
     * then the last edge must be in a cycle. */
    else if (edges[2] == 1 && edges[1] + edges[3] == 1 && edges[0] == 1) {
        for (int i : uncolored) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::CYCLE);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle and two edges are not yet colored, then one of them must be
     * in a cycle and the other must be either a matching edge or a double-star point edge. */
    else if (edges[2] == 1 && edges[0] == 2) {
        MEDTester::EdgeType option[] = {MEDTester::EdgeType::MATCHING, MEDTester::EdgeType::STAR_POINT};
        for (int i = 0; i < 2; i++) {
            MEDTester::CubicGraph::colorEdge(v, uncolored.at(i), MEDTester::EdgeType::CYCLE);
            for (int j = 0; j < 2; j++) {
                MEDTester::CubicGraph::colorEdge(v, uncolored.at((i+1)%2), option[j]);
                MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
            }
        }
    }

    /* If one edge is a double-star center edge, then other two edges must be double-star point edges. */
    else if (edges[4] == 1 && edges[0] + edges[3] == 2) {
        // First we check, if there is no adjacent double-star.
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_POINT || mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_CENTER) return;
            }
        }
        for (int i : uncolored) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::STAR_POINT);
        }
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If two edges are double-star point edges, then the remaining one must be a double-star center edge. */
    else if (edges[3] == 2 && edges[0] == 1) {
        // First we check, if there is no adjacent double-star.
        for (int i = 0; i < 3; i++) {
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_POINT) {
                for (int j = 0; j < 3; j++) {
                    if (j != mAdjListIndices[mAdjList[v][i]][v] && (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_POINT || mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_CENTER)) return;
                }
            }
        }
        MEDTester::CubicGraph::colorEdge(v, uncolored.at(0), MEDTester::EdgeType::STAR_POINT);
        MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
    }

    /* If one edge is a double-star point edge and two edges are not yet colored, then either they must be
     * cycle edges, or one of them is a double-star point edge and the other is a double-star center edge. */
    else if (edges[3] == 1 && edges[0] == 2) {
        // We check, which uncolored edges can be double-star point (that is, if there is no adjacent double-star).
        std::vector<int> canBeSEdges;
        bool canCEdges = true;
        for (int i = 0; i < 3; i++) {
            if (mDecomposition[v][i] == MEDTester::EdgeType::NONE) {
                bool canBeS = true;
                for (int j = 0; j < 3; j++) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_POINT || mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::STAR_CENTER) {
                        canBeS = false; break;
                    }
                }
                if (canBeS) canBeSEdges.push_back(i);
            } else {
                for (int j = 0; j < 3; j++) {
                    if (mDecomposition[mAdjList[v][i]][j] == MEDTester::EdgeType::CYCLE) canCEdges = false;
                }
            }
        }
        if (canCEdges) {
            for (int i : uncolored) MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::CYCLE);
            MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
        }
        for (int i : canBeSEdges) {
            MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::STAR_POINT);
            MEDTester::CubicGraph::colorEdge(v, (i == uncolored.at(0)) ? uncolored.at(1) : uncolored.at(0), MEDTester::EdgeType::STAR_CENTER);
            MEDTester::CubicGraph::decompositionHelper(index + 1, vertices);
        }
    }

    if (!mColoringDone) for (int i : uncolored) MEDTester::CubicGraph::colorEdge(v, i, MEDTester::EdgeType::NONE);
}


bool MEDTester::CubicGraph::checkCycles() const
{
    bool* checked = new bool[mVerticesCount]();

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
                    delete[] checked;
                    return false;
                }
            }
        }
    }
    delete[] checked;
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
            if (mDecomposition[v][i] == MEDTester::EdgeType::STAR_POINT) {
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
