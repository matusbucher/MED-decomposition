#include "cubic_graph.h"

#include <iostream>
#include <algorithm>
#include <queue>
#include <cassert>

cubicGraph::cubicGraph(int n) : numVertices(std::max(n, 0)) {
    vertices = new vertex*[n];
    for (int i = 0; i < n; i++) vertices[i] = new vertex(i);
}

cubicGraph::~cubicGraph() {
    for (int i = 0; i < numVertices; i++) delete vertices[i];
    delete[] vertices;
}

void cubicGraph::dfsHelper(int v, std::vector<int> &vec, std::unordered_set<int> &visited) {
    if (visited.find(v) != visited.end()) return;

    vec.push_back(v);
    visited.insert(v);

    for (int i = 0; i < 3; i++) {
        dfsHelper(vertices[v]->adjVertices[i]->id, vec, visited);
    }
}

void cubicGraph::readGraph() {
    for (int v = 0; v < numVertices; v++) {
        for (int i = 0; i < 3; i++) {
            int u; std::cin >> u;
            assert(u >= 0 && u <= numVertices-1 && u != v);
            vertices[v]->adjVertices[i] = vertices[u];
        }
    }
}

void cubicGraph::printGraph() {
    // TODO
}

std::vector<int> cubicGraph::bfs(int v) {
    std::vector<int> returnVec;
    std::unordered_set<int> visited;
    std::queue<int> q;

    q.push(v);

    while (!q.empty()) {
        if (visited.find(q.front()) != visited.end()) {
            q.pop();
            continue;
        }

        returnVec.push_back(q.front());

        for (int i = 0; i < 3; i++) {
            q.push(vertices[q.front()]->adjVertices[i]->id);
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
