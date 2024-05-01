#include <CubicGraph.h>

#include <algorithm>
#include <queue>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <iostream>


const CubicGraph::EdgeColor VERTEX_COLORINGS[3][3] = {
    {CubicGraph::CubicGraph::EdgeColor::MATCHING, CubicGraph::EdgeColor::CYCLE,CubicGraph::EdgeColor::CYCLE},
    {CubicGraph::EdgeColor::STAR_CENTER, CubicGraph::EdgeColor::STAR_POINT, CubicGraph::EdgeColor::STAR_POINT},
    {CubicGraph::EdgeColor::STAR_POINT, CubicGraph::EdgeColor::CYCLE, CubicGraph::EdgeColor::CYCLE}
};


CubicGraph::CubicGraph(const std::vector<std::vector<int>>& adjList, bool checkGraph = false)
{
    mVerticesCount = adjList.size();
    mAdjList = std::vector<std::vector<int>>(mVerticesCount, std::vector<int>(3));
    mAdjListIndices = std::vector<std::vector<int>>(mVerticesCount, std::vector<int>(mVerticesCount, -1));

    for (int u = 0; u < mVerticesCount; ++u) {
        for (int i = 0; i < 3; ++i) {
            int v = adjList[u][i];
            mAdjList[u][i] = v;
            mAdjListIndices[u][v] = i;
        }
    }

    mMedColoring = std::vector<std::vector<CubicGraph::EdgeColor>>();
    mColoringDone = false;
    mBridgesCount = -1;
    mHasBridges = false;
}

CubicGraph::~CubicGraph() {}


int CubicGraph::getVerticesCount()
{
    return mVerticesCount;
}

std::vector<std::vector<int>> CubicGraph::getAdjList()
{
    return mAdjList;
}

std::vector<std::vector<CubicGraph::EdgeColor>> CubicGraph::getMedColoring()
{
    if (mMedColoring.empty()) {
        CubicGraph::generateMedColoring(0);
    }
    return mMedColoring;
}


bool CubicGraph::printGraph(std::ostream& out)
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


std::vector<int> CubicGraph::bfs(int vertex)
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

std::vector<int> CubicGraph::dfs(int vertex)
{
    std::vector<int> returnVector;
    std::unordered_set<int> visited;
    CubicGraph::dfsHelper(vertex, returnVector, visited);

    return returnVector;
}

void CubicGraph::dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited)
{
    if (vertex < 0 || visited.find(vertex) != visited.end()) return;

    vec.push_back(vertex);
    visited.insert(vertex);

    for (int i = 0; i < 3; i++) {
        CubicGraph::dfsHelper(mAdjList[vertex][i], vec, visited);
    }
}


bool CubicGraph::isMedDecomposable() {
    if (mMedColoring.empty()) {
        CubicGraph::generateMedColoring(0);
    }
    if (mMedColoring[0][0] == CubicGraph::EdgeColor::NONE) return false;
    return true;
}

bool CubicGraph::isBridgeless() {
    if (mBridgesCount == -1) CubicGraph::countBridges(true);
    return mHasBridges;
}


void CubicGraph::colorEdge(int vertex, int index, CubicGraph::EdgeColor color)
{
    if (mColoringDone) return;
    mMedColoring[vertex][index] = color;
    mMedColoring[mAdjList[vertex][index]][mAdjListIndices[mAdjList[vertex][index]][vertex]] = color;
}

void CubicGraph::generateMedColoring(int vertex = 0)
{
    mMedColoring = std::vector<std::vector<CubicGraph::EdgeColor>>(mVerticesCount, std::vector<CubicGraph::EdgeColor>(3, CubicGraph::EdgeColor::NONE));
    mColoringDone = false;
    std::vector<int> vertices = CubicGraph::dfs(vertex);

    CubicGraph::medColoringHelper(0, vertices);
}

void CubicGraph::medColoringHelper(int index, std::vector<int>& vertices)
{
    if (mColoringDone) return;
    
    if (index == (int) vertices.size()) {
        mColoringDone = CubicGraph::checkCycles() && CubicGraph::checkDoubleStars();
        return;
    }

    int v = vertices.at(index);
    std::vector<int> uncolored;
    int edges[5];
    memset(edges, 0, sizeof(edges));

    for (int i = 0; i < 3; i++) {
        switch (mMedColoring[v][i]) {
            case CubicGraph::EdgeColor::NONE:
                edges[0]++;
                uncolored.push_back(i);
                break;
            case CubicGraph::EdgeColor::MATCHING:
                edges[1]++;
                break;
            case CubicGraph::EdgeColor::CYCLE:
                edges[2]++;
                break;
            case CubicGraph::EdgeColor::STAR_POINT:
                edges[3]++;
                break;
            case CubicGraph::EdgeColor::STAR_CENTER:
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
            if (mMedColoring[v][i] == CubicGraph::EdgeColor::STAR_CENTER) {
                for (int j = 0; j < 3; j++) {
                    if (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::MATCHING || mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::CYCLE) {
                        return;
                    }
                }
            }
        }
    }

    /* No edge of vertex was colored yet -> choose random (but correct) coloring on edges. */
    if (edges[0] == 3) {
        for (const CubicGraph::EdgeColor* colors : VERTEX_COLORINGS) {
            for (int r = 0; r < 3; r++) {
                for (int i : uncolored) {
                    CubicGraph::colorEdge(v, i, colors[(i+r) % 3]);
                }
                CubicGraph::medColoringHelper(index + 1, vertices);
            }
        }
    }

    /* If two edges are in a cycle, then the remaining one must be either a matching edge or
     * a double-star point edge. */
    else if (edges[2] == 2 && edges[0] + edges[1] + edges[3] == 1) {
        bool canSEdge = true;
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::CYCLE || mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::MATCHING) {
                    canSEdge = false;
                }
            }
            if (canSEdge) {
                CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::STAR_POINT);
                CubicGraph::medColoringHelper(index + 1, vertices);
            }
            CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::MATCHING);
        }
        CubicGraph::medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a matching edge, then other two edges must be in a cycle. */
    else if (edges[1] == 1 && edges[0] + edges[2] == 2) {
        for (int i : uncolored) {
            CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::CYCLE);
        }
        CubicGraph::medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle, one edge is either a matching or a double-star point edge,
     * then the last edge must be in a cycle. */
    else if (edges[2] == 1 && edges[1] + edges[3] == 1 && edges[0] == 1) {
        for (int i : uncolored) {
            CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::CYCLE);
        }
        CubicGraph::medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle and two edges are not yet colored, then one of them must be
     * in a cycle and the other must be either a matching edge or a double-star point edge. */
    else if (edges[2] == 1 && edges[0] == 2) {
        CubicGraph::EdgeColor option[] = {CubicGraph::EdgeColor::MATCHING, CubicGraph::EdgeColor::STAR_POINT};
        for (int i = 0; i < 2; i++) {
            CubicGraph::colorEdge(v, uncolored.at(i), CubicGraph::EdgeColor::CYCLE);
            for (int j = 0; j < 2; j++) {
                CubicGraph::colorEdge(v, uncolored.at((i+1)%2), option[j]);
                CubicGraph::medColoringHelper(index + 1, vertices);
            }
        }
    }

    /* If one edge is a double-star center edge, then other two edges must be double-star point edges. */
    else if (edges[4] == 1 && edges[0] + edges[3] == 2) {
        // First we check, if there is no adjacent double-star.
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_POINT || mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_CENTER) return;
            }
        }
        for (int i : uncolored) {
            CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::STAR_POINT);
        }
        CubicGraph::medColoringHelper(index + 1, vertices);
    }

    /* If two edges are double-star point edges, then the remaining one must be a double-star center edge. */
    else if (edges[3] == 2 && edges[0] == 1) {
        // First we check, if there is no adjacent double-star.
        for (int i = 0; i < 3; i++) {
            if (mMedColoring[v][i] == CubicGraph::EdgeColor::STAR_POINT) {
                for (int j = 0; j < 3; j++) {
                    if (j != mAdjListIndices[mAdjList[v][i]][v] && (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_POINT || mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_CENTER)) return;
                }
            }
        }
        CubicGraph::colorEdge(v, uncolored.at(0), CubicGraph::EdgeColor::STAR_POINT);
        CubicGraph::medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a double-star point edge and two edges are not yet colored, then either they must be
     * cycle edges, or one of them is a double-star point edge and the other is a double-star center edge. */
    else if (edges[3] == 1 && edges[0] == 2) {
        // We check, which uncolored edges can be double-star point (that is, if there is no adjacent double-star).
        std::vector<int> canBeSEdges;
        bool canCEdges = true;
        for (int i = 0; i < 3; i++) {
            if (mMedColoring[v][i] == CubicGraph::EdgeColor::NONE) {
                bool canBeS = true;
                for (int j = 0; j < 3; j++) {
                    if (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_POINT || mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::STAR_CENTER) {
                        canBeS = false; break;
                    }
                }
                if (canBeS) canBeSEdges.push_back(i);
            } else {
                for (int j = 0; j < 3; j++) {
                    if (mMedColoring[mAdjList[v][i]][j] == CubicGraph::EdgeColor::CYCLE) canCEdges = false;
                }
            }
        }
        if (canCEdges) {
            for (int i : uncolored) CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::CYCLE);
            CubicGraph::medColoringHelper(index + 1, vertices);
        }
        for (int i : canBeSEdges) {
            CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::STAR_POINT);
            CubicGraph::colorEdge(v, (i == uncolored.at(0)) ? uncolored.at(1) : uncolored.at(0), CubicGraph::EdgeColor::STAR_CENTER);
            CubicGraph::medColoringHelper(index + 1, vertices);
        }
    }

    if (!mColoringDone) for (int i : uncolored) CubicGraph::colorEdge(v, i, CubicGraph::EdgeColor::NONE);
}


bool CubicGraph::checkCycles()
{
    bool* checked = new bool[mVerticesCount]();

    for (int v = 0; v < mVerticesCount; v++) {
        if (!checked[v]) {
            checked[v] = true;
            bool isCycle = false;
            int next = -1;
            for (int j = 0; j < 3; j++) {
                if (mMedColoring[v][j] == CubicGraph::EdgeColor::CYCLE) {
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
                        if (mMedColoring[next][j] == CubicGraph::EdgeColor::CYCLE && mAdjList[next][j] != prev) {
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

bool CubicGraph::checkDoubleStars()
{
    std::unordered_set<int> starVertices;

    for (int v = 0; v < mVerticesCount; v++) {
        bool isStarVertex = false;
        for (int i = 0; i < 3; i++) {
            if (mMedColoring[v][i] == CubicGraph::EdgeColor::STAR_CENTER) {
                isStarVertex = false;
                break;
            }
            if (mMedColoring[v][i] == CubicGraph::EdgeColor::STAR_POINT) {
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


void CubicGraph::countBridges(bool foundOnlyOne = false)
{
    mBridgesCount = 0;
    for (int i = 0; i < mVerticesCount-1; i++) {
        for (int j = i+1; j < mVerticesCount; j++) {
            if (mAdjListIndices[i][j] != -1) {
                mAdjList[i][mAdjListIndices[i][j]] = -1;
                int n = CubicGraph::dfs(i).size();
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
