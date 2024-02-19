#include <graph_analysis.h>
#include <messages.h>
#include <cubic_graph.h>
#include <output_modes.h>

#include <iostream>
#include <chrono>

void analyzeGraphs(std::istream& in, std::ostream& out, OutputMode mode, bool showTime) {
    auto start_time = std::chrono::high_resolution_clock::now();

    switch (mode)
    {
    case OutputMode::ONLY_RESULT:
        onlyResultMode(in, out);
        break;

    case OutputMode::NOT_COLORABLE:
        notColorableMode(in, out);
        break;

    case OutputMode::NOT_COLORABLE_BRIDGELESS:
        notColorableBridgelessMode(in, out);
        break;

    case OutputMode::COLORING:
        coloringMode(in, out);
        break;
    
    default:
        std::cerr << NOT_IMPLEMENTED_MODE_MESSAGE;
        break;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    if (showTime) {
        auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(start_time - end_time);
        out << "Execution time: " << execution_time.count() << "milliseconds.\n";
    }
}

void onlyResultMode(std::istream& in, std::ostream& out) {
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

void notColorableMode(std::istream& in, std::ostream& out) {
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

void notColorableBridgelessMode(std::istream& in, std::ostream& out) {
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

void coloringMode(std::istream& in, std::ostream& out) {
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
