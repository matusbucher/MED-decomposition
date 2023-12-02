#include "cubic_graph.cpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <chrono>

char* getCmdOption(char** begin, char** end, const std::string& option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) return *itr;
    return nullptr;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

void analyzeGraphs(std::istream& in, std::ostream& out, char outMode) {
    if (!(in.good() && out.good())) {
        if (!in.good()) std::cerr << "Error with input stream\n";
        if (!in.good()) std::cerr << "Error with output stream\n";
        exit(2);
    }

    int t; in >> t;
    std::vector<int> notColorable;
    std::vector<int> notColorableBridgeless;

    while (t--) {
        int n; in >> n;
        int numV; in >> numV;
        cubicGraph graph(numV);
        graph.readGraph(in);

        switch (outMode)
        {
        case 'r':
            out << "Graph " << n << ": " << ((graph.isMedDecomposable() ? "true" : "false")) << "\n";
            break;
        
        case 'c':
            out << "MED coloring for graph " << n << ":\n";
            for (int v = 0; v < numV; v++) {
                out << v << ":";
                for (int i = 0; i < 3; i++) out << " " << graph.getMedColoring()[v][i];
                out << "\n";
            }
            break;

        case 'n':
            if (!graph.isMedDecomposable()) notColorable.push_back(n);
            break;

        case 'b':
            if (!graph.isMedDecomposable() && graph.isBridgeless()) notColorableBridgeless.push_back(n);
            break;
        
        default:
            std::cerr << "Unknown output option '" << outMode << "'\n";
            exit(2);
        }
    }

    if (outMode == 'n') {
        out << "Not MED decomposable graphs:\n";
        for (int g : notColorable) out << g << "\n";
    }

    if (outMode == 'b') {
        out << "Not MED decomposable 2-edge-connected graphs:\n";
        for (int g : notColorableBridgeless) out << g << "\n";
    }
}


int main(int argc, char *argv[]) {

    char* inFileName = getCmdOption(argv, argv + argc, "-i");
    char* outFileName = getCmdOption(argv, argv + argc, "-o");
    char* ownMode = getCmdOption(argv, argv + argc, "-m");

    if (cmdOptionExists(argv, argv + argc, "-i") && inFileName == nullptr) {
        std::cerr << "Invalid syntax: Use option -i with a path to input file\n";
        exit(1);
    }

    if (cmdOptionExists(argv, argv + argc, "-o") && outFileName == nullptr) {
        std::cerr << "Invalid syntax: Use option -o with a path to output file\n";
        exit(1);
    }

    if (cmdOptionExists(argv, argv + argc, "-m") && ownMode == nullptr) {
        std::cerr << "Invalid syntax: Use option -m with a character representation of mode ( r / n / c )\n";
        exit(1);
    }

    std::ifstream inFile;
    if (inFileName != nullptr) {
        if (!std::filesystem::exists(inFileName)) {
            std::cerr << "No such input file: '" << inFileName << "'\n";
            exit(1);
        }
        inFile.open(inFileName);
        if (!inFile.is_open()) {
            std::cerr << "Error with opening input file: " << inFileName << "\n";
            exit(1);
        }
    }

    std::ofstream outFile;
    if (outFileName != nullptr) {
        outFile.open(outFileName);
        if (!outFile.is_open()) {
            std::cerr << "Error with opening output file: " << outFileName << "\n";
            exit(1);
        }
    }
    
    std::istream& in = (inFile.is_open()) ? inFile : std::cin;
    std::ostream& out = (outFile.is_open()) ? outFile : std::cout;

    char mode = 'r';
    if (ownMode != nullptr) {
        if (std::strlen(ownMode) > 1 || (ownMode[0] != 'r' && ownMode[0] != 'n' && ownMode[0] != 'c' && ownMode[0] != 'b')) {
            std::cerr << "Invalid output mode specifier: " << ownMode << " (use 'r' / 'n' / 'c' / 'b')\n";
            exit(1);
        }
        mode = ownMode[0];
    }

    if (cmdOptionExists(argv, argv + argc, "-t")) {
        auto start = std::chrono::high_resolution_clock::now();
        analyzeGraphs(in, out, mode);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Time taken: " << duration.count() << " miliseconds\n";
    } else {
        analyzeGraphs(in, out, mode);
    }

    return 0;
}
