#ifndef CUBIC_GRAPH_H
#define CUBIC_GRAPH_H

#include <vector>
#include <unordered_set>

struct vertex {
    int id;
    vertex *adjVertices[3];

    vertex(int _id = 0) : id{_id} { adjVertices[0] = adjVertices[1] = adjVertices[2] = nullptr; }
};


class cubicGraph {

private:
    int numVertices;
    vertex **vertices;

    void dfsHelper(int, std::vector<int>&, std::unordered_set<int>&);

public:
    cubicGraph(int n);
    ~cubicGraph();

    void readGraph();
    void printGraph();

    std::vector<int> bfs(int v);
    std::vector<int> dfs(int v);
};

#endif