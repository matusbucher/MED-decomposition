#include <graph_analysis.h>
#include <messages.h>
#include <CubicGraph.h>
#include <OutputMode.h>

#include <iostream>
#include <chrono>

void graph_analysis::analyzeGraphs(std::istream& in, std::ostream& out, OutputMode mode, bool showTime) {
    auto start_time = std::chrono::high_resolution_clock::now();

    switch (mode)
    {
    case OutputMode::ONLY_RESULT:
        graph_analysis::onlyResultMode(in, out);
        break;

    case OutputMode::NOT_COLORABLE:
        graph_analysis::notColorableMode(in, out);
        break;

    case OutputMode::NOT_COLORABLE_BRIDGELESS:
        graph_analysis::notColorableBridgelessMode(in, out);
        break;

    case OutputMode::COLORING:
        graph_analysis::coloringMode(in, out);
        break;
    
    default:
        std::cerr << message::NOT_IMPLEMENTED_MODE;
        break;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    if (showTime) {
        auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        out << "Execution time: " << execution_time.count() << " milliseconds.\n";
    }
}

void graph_analysis::onlyResultMode(std::istream& in, std::ostream& out) {
    int graphsCount;
    in >> graphsCount;

    while (graphsCount--) {
        int graphNum, verticesCount;
        in >> graphNum >> verticesCount;

        CubicGraph graph(verticesCount);
        graph.readGraph(in);

        out << "Graph " << graphNum << ": " << ((graph.isMedDecomposable() ? "true" : "false")) << "\n";
    }
}

void graph_analysis::notColorableMode(std::istream& in, std::ostream& out) {
    int graphsCount;
    in >> graphsCount;

    out << "Not MED decomposable graphs:\n";

    while (graphsCount--) {
        int graphNum, verticesCount;
        in >> graphNum >> verticesCount;

        CubicGraph graph(verticesCount);
        graph.readGraph(in);

        if (!graph.isMedDecomposable()) {
            out << graphNum << "\n";
        }
    }
}

void graph_analysis::notColorableBridgelessMode(std::istream& in, std::ostream& out) {
    int graphsCount;
    in >> graphsCount;

    out << "Not MED decomposable and bridgeless graphs:\n";

    while (graphsCount--) {
        int graphNum, verticesCount;
        in >> graphNum >> verticesCount;

        CubicGraph graph(verticesCount);
        graph.readGraph(in);

        if (graph.isBridgeless() && !graph.isMedDecomposable()) {
            out << graphNum << "\n";
        }
    }
}

void graph_analysis::coloringMode(std::istream& in, std::ostream& out) {
    int graphsCount;
    in >> graphsCount;

    while (graphsCount--) {
        int graphNum, verticesCount;
        in >> graphNum >> verticesCount;

        CubicGraph graph(verticesCount);
        graph.readGraph(in);

        out << "Graph " << graphNum << ":\n";

        if (graph.isMedDecomposable()) {
            int** adjList = graph.getAdjList();
            EdgeColor** medColoring = graph.getMedColoring();

            for (int i = 0; i < graph.getVerticesCount(); i++) {
                out << i << ":";
                for (int j = 0; j < 3; j++) {
                    out << " " << adjList[i][j];
                    switch (medColoring[i][j]) {
                        case EdgeColor::MATCHING:
                            out << "m";
                            break;
                        case EdgeColor::CYCLE:
                            out << "c";
                            break;
                        case EdgeColor::STAR_POINT:
                            out << "h";
                            break;
                        case EdgeColor::STAR_CENTER:
                            out << "s";
                            break;
                        default:
                            std::abort();
                            break;
                    }
                }
                out << "\n";
            }
        } else {
            out << "false\n";
        }
    }
}
