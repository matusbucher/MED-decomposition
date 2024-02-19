#include <cubic_graph.h>

#include <algorithm>
#include <queue>
#include <cassert>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <iostream>

const EdgeColor VERTEX_COLORINGS[3][3] = {
    {EdgeColor::MATCHING, EdgeColor::CYCLE,EdgeColor::CYCLE},
    {EdgeColor::STAR_CENTER, EdgeColor::STAR_POINT, EdgeColor::STAR_POINT},
    {EdgeColor::STAR_POINT, EdgeColor::CYCLE, EdgeColor::CYCLE}
};

CubicGraph::CubicGraph(int n) {
    verticesCount = std::max(n, 4);
    adjList = new int*[verticesCount];
    adjListIndices = new int*[verticesCount];
    for (int i = 0; i < verticesCount; i++) {
        adjList[i] = new int[3];
        adjListIndices[i] = new int[verticesCount];
    }
    medColoring = nullptr;
    coloringDone = false;
    bridgesCount = -1;
}

CubicGraph::~CubicGraph() {
    for (int i = 0; i < verticesCount; i++) {
        delete[] adjList[i];
        delete[] adjListIndices[i];
        if (medColoring != nullptr) delete[] medColoring[i];
    }
    delete[] adjList;
    delete[] adjListIndices;
    if (medColoring != nullptr) delete[] medColoring;
}

int CubicGraph::getVerticesCount() {
    return verticesCount;
}

int** CubicGraph::getAdjList() {
    return adjList;
}

void CubicGraph::readGraph(std::istream& in) {
    for (int i = 0; i < verticesCount; i++) {
        for (int j = 0; j < verticesCount; j++) adjListIndices[i][j] = -1;
    }

    for (int u = 0; u < verticesCount; u++) {
        for (int i = 0; i < 3; i++) {
            if (!in.good()) {
                std::cerr << "Error reading graph\n";
                return;
            }
            int v; in >> v;
            assert(v >= 0 && v <= verticesCount-1 && v != u && "Invalid vertex input");
            adjList[u][i] = v;
            adjListIndices[u][v] = i;
        }
    }

    if (medColoring != nullptr) {
        for (int i = 0; i < verticesCount; i++) {
            delete[] medColoring[i];
        }
        delete[] medColoring;
        medColoring = nullptr;
    }
    bridgesCount = -1;
}

void CubicGraph::printGraph(std::ostream& out) {
    if (!out.good()) {
        std::cerr << "Error printing graph\n";
        return;
    }
    out << "Printing graph as adjacency list:\n";
    for (int i = 0; i < verticesCount; i++) {
        out << i << ":";
        for (int j = 0; j < 3; j++) {
            out << " " << adjList[i][j];
        }
        out << std::endl;
    }
}

std::vector<int> CubicGraph::bfs(int vertex) {
    std::vector<int> returnVec;
    std::unordered_set<int> visited;
    std::queue<int> q;

    q.push(vertex);

    while (!q.empty()) {
        if (q.front() < 0 || visited.find(q.front()) != visited.end()) {
            q.pop();
            continue;
        }

        returnVec.push_back(q.front());

        for (int i = 0; i < 3; i++) {
            q.push(adjList[q.front()][i]);
        }

        visited.insert(q.front());
        q.pop();
    }

    return returnVec;
}

std::vector<int> CubicGraph::dfs(int vertex) {
    std::vector<int> returnVec;
    std::unordered_set<int> visited;
    dfsHelper(vertex, returnVec, visited);

    return returnVec;
}

void CubicGraph::dfsHelper(int vertex, std::vector<int>& vec, std::unordered_set<int>& visited) {
    if (vertex < 0 || visited.find(vertex) != visited.end()) return;

    vec.push_back(vertex);
    visited.insert(vertex);

    for (int i = 0; i < 3; i++) {
        dfsHelper(adjList[vertex][i], vec, visited);
    }
}

void CubicGraph::generateMedColoring(int vertex = 0) {
    if (medColoring == nullptr) {
        medColoring = new EdgeColor*[verticesCount];
        for (int i = 0; i < verticesCount; i++) {
            medColoring[i] = new EdgeColor[3];
            for (int j = 0; j < 3; j++) medColoring[i][j] = EdgeColor::NONE;
        }
        coloringDone = false;
        std::vector<int> vertices = dfs(vertex);

        medColoringHelper(0, vertices);
    }
}

void CubicGraph::colorEdge(int vertex, int index, EdgeColor color) {
    if (coloringDone) return;
    medColoring[vertex][index] = color;
    medColoring[adjList[vertex][index]][adjListIndices[adjList[vertex][index]][vertex]] = color;
}

bool CubicGraph::checkCycles() {
    bool* checked = new bool[verticesCount]();

    for (int v = 0; v < verticesCount; v++) {
        if (!checked[v]) {
            checked[v] = true;
            bool isCycle = false;
            int next = -1;
            for (int j = 0; j < 3; j++) {
                if (medColoring[v][j] == EdgeColor::CYCLE) {
                    isCycle = true;
                    next = adjList[v][j];
                    break;
                }
            }
            if (isCycle) {
                int length = 1;
                int prev = v;
                int maxIter = verticesCount;
                while (next != v && maxIter--) {
                    checked[next] = true;
                    length++;
                    for (int j = 0; j < 3; j++) {
                        if (medColoring[next][j] == EdgeColor::CYCLE && adjList[next][j] != prev) {
                            prev = next;
                            next = adjList[next][j];
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

bool CubicGraph::checkDoubleStars() {
    std::unordered_set<int> starVertices;

    for (int v = 0; v < verticesCount; v++) {
        bool isStarVertex = false;
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == EdgeColor::STAR_CENTER) { isStarVertex = false; break; }
            if (medColoring[v][i] == EdgeColor::STAR_POINT) isStarVertex = true;
        }
        if (isStarVertex) {
            for (int i = 0; i < 3; i++) {
                if (starVertices.find(adjList[v][i]) != starVertices.end()) return false;
            }
            starVertices.insert(v);
        }
    }
    return true;
}

void CubicGraph::medColoringHelper(int index, std::vector<int>& vertices) {
    if (coloringDone) return;
    
    if (index == (int) vertices.size()) {
        coloringDone = checkCycles() && checkDoubleStars();
        return;
    }

    int v = vertices.at(index);
    std::vector<int> uncolored;
    int edges[5];
    memset(edges, 0, sizeof(edges));

    for (int i = 0; i < 3; i++) {
        switch (medColoring[v][i]) {
            case EdgeColor::NONE:
                edges[0]++;
                uncolored.push_back(i);
                break;
            case EdgeColor::MATCHING:
                edges[1]++;
                break;
            case EdgeColor::CYCLE:
                edges[2]++;
                break;
            case EdgeColor::STAR_POINT:
                edges[3]++;
                break;
            case EdgeColor::STAR_CENTER:
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
            if (medColoring[v][i] == EdgeColor::STAR_CENTER) {
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == EdgeColor::MATCHING || medColoring[adjList[v][i]][j] == EdgeColor::CYCLE) {
                        return;
                    }
                }
            }
        }
    }

    /* No edge of vertex was colored yet -> choose random (but correct) coloring on edges. */
    if (edges[0] == 3) {
        for (const EdgeColor* colors : VERTEX_COLORINGS) {
            for (int r = 0; r < 3; r++) {
                for (int i : uncolored) {
                    colorEdge(v, i, colors[(i+r) % 3]);
                }
                medColoringHelper(index + 1, vertices);
            }
        }
    }

    /* If two edges are in a cycle, then the remaining one must be either a matching edge or
     * a double-star point edge. */
    else if (edges[2] == 2 && edges[0] + edges[1] + edges[3] == 1) {
        bool canSEdge = true;
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (medColoring[adjList[v][i]][j] == EdgeColor::CYCLE || medColoring[adjList[v][i]][j] == EdgeColor::MATCHING) {
                    canSEdge = false;
                }
            }
            if (canSEdge) {
                colorEdge(v, i, EdgeColor::STAR_POINT);
                medColoringHelper(index + 1, vertices);
            }
            colorEdge(v, i, EdgeColor::MATCHING);
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a matching edge, then other two edges must be in a cycle. */
    else if (edges[1] == 1 && edges[0] + edges[2] == 2) {
        for (int i : uncolored) {
            colorEdge(v, i, EdgeColor::CYCLE);
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle, one edge is either a matching or a double-star point edge,
     * then the last edge must be in a cycle. */
    else if (edges[2] == 1 && edges[1] + edges[3] == 1 && edges[0] == 1) {
        for (int i : uncolored) {
            colorEdge(v, i, EdgeColor::CYCLE);
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle and two edges are not yet colored, then one of them must be
     * in a cycle and the other must be either a matching edge or a double-star point edge. */
    else if (edges[2] == 1 && edges[0] == 2) {
        EdgeColor option[] = {EdgeColor::MATCHING, EdgeColor::STAR_POINT};
        for (int i = 0; i < 2; i++) {
            colorEdge(v, uncolored.at(i), EdgeColor::CYCLE);
            for (int j = 0; j < 2; j++) {
                colorEdge(v, uncolored.at((i+1)%2), option[j]);
                medColoringHelper(index + 1, vertices);
            }
        }
    }

    /* If one edge is a double-star center edge, then other two edges must be double-star point edges. */
    else if (edges[4] == 1 && edges[0] + edges[3] == 2) {
        // First we check, if there is no adjacent double-star.
        for (int i : uncolored) {
            for (int j = 0; j < 3; j++) {
                if (medColoring[adjList[v][i]][j] == EdgeColor::STAR_POINT || medColoring[adjList[v][i]][j] == EdgeColor::STAR_CENTER) return;
            }
        }
        for (int i : uncolored) {
            colorEdge(v, i, EdgeColor::STAR_POINT);
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If two edges are double-star point edges, then the remaining one must be a double-star center edge. */
    else if (edges[3] == 2 && edges[0] == 1) {
        // First we check, if there is no adjacent double-star.
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == EdgeColor::STAR_POINT) {
                for (int j = 0; j < 3; j++) {
                    if (j != adjListIndices[adjList[v][i]][v] && (medColoring[adjList[v][i]][j] == EdgeColor::STAR_POINT || medColoring[adjList[v][i]][j] == EdgeColor::STAR_CENTER)) return;
                }
            }
        }
        colorEdge(v, uncolored.at(0), EdgeColor::STAR_POINT);
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a double-star point edge and two edges are not yet colored, then either they must be
     * cycle edges, or one of them is a double-star point edge and the other is a double-star center edge. */
    else if (edges[3] == 1 && edges[0] == 2) {
        // We check, which uncolored edges can be double-star point (that is, if there is no adjacent double-star).
        std::vector<int> canBeSEdges;
        bool canCEdges = true;
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == EdgeColor::NONE) {
                bool canBeS = true;
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == EdgeColor::STAR_POINT || medColoring[adjList[v][i]][j] == EdgeColor::STAR_CENTER) {
                        canBeS = false; break;
                    }
                }
                if (canBeS) canBeSEdges.push_back(i);
            } else {
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == EdgeColor::CYCLE) canCEdges = false;
                }
            }
        }
        if (canCEdges) {
            for (int i : uncolored) colorEdge(v, i, EdgeColor::CYCLE);
            medColoringHelper(index + 1, vertices);
        }
        for (int i : canBeSEdges) {
            colorEdge(v, i, EdgeColor::STAR_POINT);
            colorEdge(v, (i == uncolored.at(0)) ? uncolored.at(1) : uncolored.at(0), EdgeColor::STAR_CENTER);
            medColoringHelper(index + 1, vertices);
        }
    }

    if (!coloringDone) for (int i : uncolored) colorEdge(v, i, EdgeColor::NONE);
}

EdgeColor** CubicGraph::getMedColoring() {
    if (medColoring == nullptr) {
        generateMedColoring();
    }
    return medColoring;
}

bool CubicGraph::isMedDecomposable() {
    if (medColoring == nullptr) {
        generateMedColoring();
    }
    if (medColoring[0][0] == EdgeColor::NONE) return false;
    return true;
}

void CubicGraph::countBridges() {
    bridgesCount = 0;
    for (int i = 0; i < verticesCount-1; i++) {
        for (int j = i+1; j < verticesCount; j++) {
            if (adjListIndices[i][j] != -1) {
                adjList[i][adjListIndices[i][j]] = -1;
                int n = dfs(i).size();
                adjList[i][adjListIndices[i][j]] = j;
                if (n != verticesCount) bridgesCount++;
            }
        }
    }    
}

bool CubicGraph::isBridgeless() {
    if (bridgesCount == -1) countBridges();
    return bridgesCount == 0;
}
