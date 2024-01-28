#include "cubic_graph.h"
#include "constants.h"

#include <algorithm>
#include <queue>
#include <cassert>
#include <cstring>

cubicGraph::cubicGraph(int n) {
    numVertices = std::max(n, 4);
    adjList = new int*[numVertices];
    adjListIndices = new int*[numVertices];
    for (int i = 0; i < numVertices; i++) {
        adjList[i] = new int[3];
        adjListIndices[i] = new int[numVertices];
    }
    medColoring = nullptr;
    coloringDone = false;
    hasNoBridge = -1;
}


cubicGraph::~cubicGraph() {
    for (int i = 0; i < numVertices; i++) {
        delete[] adjList[i];
        delete[] adjListIndices[i];
        if (medColoring != nullptr) delete[] medColoring[i];
    }
    delete[] adjList;
    delete[] adjListIndices;
    if (medColoring != nullptr) delete[] medColoring;
}


void cubicGraph::readGraph(std::istream& in) {
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) adjListIndices[i][j] = -1;
    }

    for (int u = 0; u < numVertices; u++) {
        for (int i = 0; i < 3; i++) {
            if (!in.good()) {
                std::cerr << "Error reading graph\n";
                return;
            }
            int v; in >> v;
            assert(v >= 0 && v <= numVertices-1 && v != u && "Invalid vertex input");
            adjList[u][i] = v;
            adjListIndices[u][v] = i;
        }
    }

    if (medColoring != nullptr) {
        for (int i = 0; i < numVertices; i++) {
            delete[] medColoring[i];
        }
        delete[] medColoring;
        medColoring = nullptr;
    }
    hasNoBridge = -1;
}


void cubicGraph::printGraph(std::ostream& out) {
    if (!out.good()) {
        std::cerr << "Error printing graph\n";
        return;
    }
    out << "Printing graph as adjacency list:\n";
    for (int i = 0; i < numVertices; i++) {
        out << i << ":";
        for (int j = 0; j < 3; j++) {
            out << " " << adjList[i][j];
        }
        out << std::endl;
    }
}


std::vector<int> cubicGraph::bfs(int v) {
    std::vector<int> returnVec;
    std::unordered_set<int> visited;
    std::queue<int> q;

    q.push(v);

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


std::vector<int> cubicGraph::dfs(int v) {
    std::vector<int> returnVec;
    std::unordered_set<int> visited;
    dfsHelper(v, returnVec, visited);

    return returnVec;
}


void cubicGraph::dfsHelper(int v, std::vector<int>& vec, std::unordered_set<int>& visited) {
    if (v < 0 || visited.find(v) != visited.end()) return;

    vec.push_back(v);
    visited.insert(v);

    for (int i = 0; i < 3; i++) {
        dfsHelper(adjList[v][i], vec, visited);
    }
}


void cubicGraph::generateMedColoring(int v = 0) {
    if (medColoring == nullptr) {
        medColoring = new char*[numVertices];
        for (int i = 0; i < numVertices; i++) {
            medColoring[i] = new char[3];
            for (int j = 0; j < 3; j++) medColoring[i][j] = '-';
        }
        coloringDone = false;
        std::vector<int> vertices = dfs(v);

        medColoringHelper(0, vertices);
    }
}


void cubicGraph::colorEdge(int v, int i, char color) {
    if (coloringDone) return;
    medColoring[v][i] = color;
    medColoring[adjList[v][i]][adjListIndices[adjList[v][i]][v]] = color;
}


bool cubicGraph::checkCycles() {
    bool* checked = new bool[numVertices]();
    for (int v = 0; v < numVertices; v++) {
        if (!checked[v]) {
            checked[v] = true;
            bool isCycle = false;
            int next = -1;
            for (int j = 0; j < 3; j++) {
                if (medColoring[v][j] == 'c') {
                    isCycle = true;
                    next = adjList[v][j];
                    break;
                }
            }
            if (isCycle) {
                int length = 1;
                int prev = v;
                int maxIter = numVertices;
                while (next != v && maxIter--) {
                    checked[next] = true;
                    length++;
                    for (int j = 0; j < 3; j++) {
                        if (medColoring[next][j] == 'c' && adjList[next][j] != prev) {
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


bool cubicGraph::checkDoubleStars() {
    std::unordered_set<int> starVertices;
    for (int v = 0; v < numVertices; v++) {
        bool isStarVertex = false;
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == 's') { isStarVertex = false; break; }
            if (medColoring[v][i] == 'h') isStarVertex = true;
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


void cubicGraph::medColoringHelper(int index, std::vector<int>& vertices) {
    if (coloringDone) return;
    
    if (index == vertices.size()) {
        coloringDone = checkCycles() && checkDoubleStars();
        return;
    }

    int v = vertices.at(index);
    std::vector<int> uncolored;
    int edges[5];
    memset(edges, 0, sizeof(edges));

    for (int i = 0; i < 3; i++) {
        switch (medColoring[v][i]) {
            case '-': edges[0]++; uncolored.push_back(i); break;
            case 'm': edges[1]++; break;
            case 'c': edges[2]++; break;
            case 'h': edges[3]++; break;
            case 's': edges[4]++; break;
            default: std::abort(); break;
        }
    }

    /* Firstly, if one of edges is double-star point edge and at least one other edge is
     * a cycle egde or matching edge, then we check, if there is no other cycle or matching
     * edge incident to the double-star edge on the other side. */
    if (edges[3] == 1 && edges[1] + edges[2] > 0) {
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == 's') {
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == 'm' || medColoring[adjList[v][i]][j] == 'c') {
                        return;
                    }
                }
            }
        }
    }

    /* No edge of vertex was colored yet -> choose random (but correct) coloring on edges. */
    if (edges[0] == 3) {
        for (const char* colors : VERTEX_COLORINGS) {
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
                if (medColoring[adjList[v][i]][j] == 'c' || medColoring[adjList[v][i]][j] == 'm') {
                    canSEdge = false;
                }
            }
            if (canSEdge) {
                colorEdge(v, i, 'h');
                medColoringHelper(index + 1, vertices);
            }
            colorEdge(v, i, 'm');
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a matching edge, then other two edges must be in a cycle. */
    else if (edges[1] == 1 && edges[0] + edges[2] == 2) {
        for (int i : uncolored) {
            colorEdge(v, i, 'c');
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle, one edge is either a matching or a double-star point edge,
     * then the last edge must be in a cycle. */
    else if (edges[2] == 1 && edges[1] + edges[3] == 1 && edges[0] == 1) {
        for (int i : uncolored) {
            colorEdge(v, i, 'c');
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is in a cycle and two edges are not yet colored, then one of them must be
     * in a cycle and the other must be either a matching edge or a double-star point edge. */
    else if (edges[2] == 1 && edges[0] == 2) {
        char option[] = {'m','h'};
        for (int i = 0; i < 2; i++) {
            colorEdge(v, uncolored.at(i), 'c');
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
                if (medColoring[adjList[v][i]][j] == 'h' || medColoring[adjList[v][i]][j] == 's') return;
            }
        }
        for (int i : uncolored) {
            colorEdge(v, i, 'h');
        }
        medColoringHelper(index + 1, vertices);
    }

    /* If two edges are double-star point edges, then the remaining one must be a double-star center edge. */
    else if (edges[3] == 2 && edges[0] == 1) {
        // First we check, if there is no adjacent double-star.
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == 'h') {
                for (int j = 0; j < 3; j++) {
                    if (j != adjListIndices[adjList[v][i]][v] && (medColoring[adjList[v][i]][j] == 'h' || medColoring[adjList[v][i]][j] == 's')) return;
                }
            }
        }
        colorEdge(v, uncolored.at(0), 's');
        medColoringHelper(index + 1, vertices);
    }

    /* If one edge is a double-star point edge and two edges are not yet colored, then either they must be
     * cycle edges, or one of them is a double-star point edge and the other is a double-star center edge. */
    else if (edges[3] == 1 && edges[0] == 2) {
        // We check, which uncolored edges can be double-star point (that is, if there is no adjacent double-star).
        std::vector<int> canBeSEdges;
        bool canCEdges = true;
        for (int i = 0; i < 3; i++) {
            if (medColoring[v][i] == '-') {
                bool canBeS = true;
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == 'h' || medColoring[adjList[v][i]][j] == 's') {
                        canBeS = false; break;
                    }
                }
                if (canBeS) canBeSEdges.push_back(i);
            } else {
                for (int j = 0; j < 3; j++) {
                    if (medColoring[adjList[v][i]][j] == 'c') canCEdges = false;
                }
            }
        }
        if (canCEdges) {
            for (int i : uncolored) colorEdge(v, i, 'c');
            medColoringHelper(index + 1, vertices);
        }
        for (int i : canBeSEdges) {
            colorEdge(v, i, 'h');
            colorEdge(v, (i == uncolored.at(0)) ? uncolored.at(1) : uncolored.at(0), 's');
            medColoringHelper(index + 1, vertices);
        }
    }

    if (!coloringDone) for (int i : uncolored) colorEdge(v, i, '-');
}


char** cubicGraph::getMedColoring() {
    if (medColoring == nullptr) {
        generateMedColoring();
    }
    return medColoring;
}


bool cubicGraph::isMedDecomposable() {
    if (medColoring == nullptr) {
        generateMedColoring();
    }
    if (medColoring[0][0] == '-') return false;
    return true;
}


bool cubicGraph::isBridgeless() {
    if (hasNoBridge == -1) {
        hasNoBridge = 1;
        for (int i = 0; i < numVertices-1; i++) {
            for (int j = i+1; j < numVertices; j++) {
                if (adjListIndices[i][j] != -1) {
                    adjList[i][adjListIndices[i][j]] = -1;
                    int n = dfs(i).size();
                    adjList[i][adjListIndices[i][j]] = j;
                    if (n != numVertices) {
                        hasNoBridge = 0;
                        goto isBridgelessReturn;
                    }
                }
            }
        }
    }
    isBridgelessReturn: return (bool) hasNoBridge;
}