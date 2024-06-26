#include "GraphAnalyser.h"

#ifdef SAT
    #include "SatSolver.h"
#else
    #include "CubicGraph.h"
#endif

#include "typedefs.h"
#include "OutputMode.h"
#include "Parser.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>


MEDTester::GraphAnalyser::GraphAnalyser(MEDTester::Parser& parser)
{
    parser.checkSyntax();
    parser.parseAll();

    mInputFilename = parser.getInputFilename();
    mOutputFilename = parser.getOutputFilename();
    mOutputMode = parser.getOutputMode();
    mShowTime = parser.getShowTime();
}

MEDTester::GraphAnalyser::GraphAnalyser(const std::string& inputFilename, const std::string& outputFilename, MEDTester::OutputMode outputMode, bool showTime)
{
    mInputFilename = inputFilename;
    mOutputFilename = outputFilename;
    mOutputMode = outputMode;
    mShowTime = showTime;
}

MEDTester::GraphAnalyser::~GraphAnalyser() {}


void MEDTester::GraphAnalyser::analyze() const
{
    std::istream *in;
    std::ifstream inputFile;
    if (mInputFilename == MEDTester::Parser::INPUT_FILENAME_OPTION_INFO.defaultValue) {
        in = &std::cin;
    } else {
        if (!std::filesystem::exists(mInputFilename)) {
            throw FileErrorException(INPUT_FILE_DOES_NOT_EXIST_MESSAGE(mInputFilename));
        }
        inputFile.open(mInputFilename, std::ifstream::in);
        if (!inputFile.good()) {
            throw FileErrorException(CANNOT_OPEN_FILE_MESSAGE(mInputFilename));
        }
        in = &inputFile;
    }

    std::ostream *out;
    std::ofstream outputFile;
    if (mOutputFilename == MEDTester::Parser::OUTPUT_FILENAME_OPTION_INFO.defaultValue) {
        out = &std::cout;
    } else {
        outputFile.open(mOutputFilename, std::ofstream::out);
        if (!outputFile.is_open())
            throw FileErrorException(CANNOT_OPEN_FILE_MESSAGE(mOutputFilename));
        out = &outputFile;
    }

    auto timeStart = std::chrono::high_resolution_clock::now();

    switch (mOutputMode)
    {
    case MEDTester::OutputMode::ONLY_RESULT: 
        onlyResultMode(*in, *out);
        break;

    case MEDTester::OutputMode::NOT_DECOMPOSABLE: 
        notDecomposableMode(*in, *out);
        break;

    case MEDTester::OutputMode::NOT_DECOMPOSABLE_BRIDGELESS: 
        notDecomposableBridgelessMode(*in, *out);
        break;

    case MEDTester::OutputMode::COLORING: 
        coloringMode(*in, *out);
        break;
    }

    auto timeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
    if (mShowTime) {
        *out << "execution time: " << executionTime.count() << " milliseconds\n";
    }
    
    inputFile.close();
    outputFile.close();
}


void MEDTester::GraphAnalyser::onlyResultMode(std::istream& in, std::ostream& out)
{
    int graphCount = getInt(in, "number of graphs");
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = getInt(in, "graph number, " + std::to_string(i) + ". graph");
        MEDTester::CubicGraph graph(getAdjList(in, graphNum, true));

        #ifdef SAT
            MEDTester::SatSolver solver(graph);
            out << graphNum << ": " << (solver.isDecomposable() ? "true" : "false") << "\n";
        #else
            out << graphNum << ": " << (graph.isDecomposable() ? "true" : "false") << " (graph)\n";
        #endif
    }
}

void MEDTester::GraphAnalyser::notDecomposableMode(std::istream& in, std::ostream& out)
{
    int graphCount = getInt(in, "number of graphs");
    out << "Not decomposable graphs:\n";
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = getInt(in, "graph number, " + std::to_string(i) + ". graph");
        MEDTester::CubicGraph graph(getAdjList(in, graphNum, true));

        #ifdef SAT
            MEDTester::SatSolver solver(graph);
            if (!solver.isDecomposable()) out << graphNum << "\n";
        #else
            if (!graph.isDecomposable()) out << graphNum << "\n";
        #endif
    }
}

void MEDTester::GraphAnalyser::notDecomposableBridgelessMode(std::istream& in, std::ostream& out)
{
    int graphCount = getInt(in, "number of graphs");
    out << "Not decomposable and bridgeless graphs:\n";
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = getInt(in, "graph number, " + std::to_string(i) + ". graph");
        MEDTester::CubicGraph graph(getAdjList(in, graphNum, true));

        if (graph.isBridgeless()) {
            #ifdef SAT
                MEDTester::SatSolver solver(graph);
                if (!solver.isDecomposable()) out << graphNum << "\n";
            #else
                if (!graph.isDecomposable()) out << graphNum << "\n";
            #endif
        }
    }
}

void MEDTester::GraphAnalyser::coloringMode(std::istream& in, std::ostream& out)
{
    const std::unordered_map<MEDTester::EdgeType, std::string> edgeColorChar = {
        {MEDTester::EdgeType::NONE, "-"},
        {MEDTester::EdgeType::MATCHING, "M"},
        {MEDTester::EdgeType::CYCLE, "C"},
        {MEDTester::EdgeType::STAR_LEAF, "H"},
        {MEDTester::EdgeType::STAR_CENTER, "S"}
    };

    int graphCount = getInt(in, "number of graphs");
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = getInt(in, "graph number, " + std::to_string(i) + ". graph");
        MEDTester::Matrix adjList = getAdjList(in, graphNum, true);
        MEDTester::CubicGraph graph(adjList);

        #ifdef SAT
            MEDTester::SatSolver solver(graph);
            out << "graph " << graphNum << ":\n";
            if (solver.isDecomposable()) {
                MEDTester::Decomposition coloring = solver.getDecomposition();
                for (int i = 0; i < (int) adjList.size(); ++i) {
                    out << adjList[i][0] << edgeColorChar.at(coloring[i][0]);
                    for (int j = 1; j < 3; ++j) {
                        out << " " << adjList[i][j] << edgeColorChar.at(coloring[i][j]);
                    }
                    out << "\n";
                }
            } else {
                out << "false\n";
            }
        #else
            out << "graph " << graphNum << ":\n";
            if (graph.isDecomposable()) {
                MEDTester::Decomposition coloring = graph.getDecomposition();
                for (int i = 0; i < graph.getVerticesCount(); ++i) {
                    out << adjList[i][0] << edgeColorChar.at(coloring[i][0]);
                    for (int j = 1; j < 3; ++j) {
                        out << " " << adjList[i][j] << edgeColorChar.at(coloring[i][j]);
                    }
                    out << "\n";
                }
            } else {
                out << "false\n";
            }
        #endif
    }
}


inline std::string MEDTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(const std::string& additionalInfo)
{
    return "Wrong input format (" + additionalInfo + "). Use '" + MEDTester::Parser::HELP_SPECIFIER + "' for command description (see 'Input Format').";
}

inline std::string MEDTester::GraphAnalyser::INPUT_FILE_DOES_NOT_EXIST_MESSAGE(const std::string& filename)
{
    return "Specified input file '" + filename + "' does not exist.";
}

inline std::string MEDTester::GraphAnalyser::CANNOT_OPEN_FILE_MESSAGE(const std::string& filename)
{
    return "Cannot open specified file '" + filename + "'.";
}

int MEDTester::GraphAnalyser::getInt(std::istream& in, const std::string& what)
{
    int x;
    if (!(in >> x)) {
        throw WrongInputException(WRONG_INPUT_FORMAT_MESSAGE("missing " + what));
    }
    return x;
}

MEDTester::Matrix MEDTester::GraphAnalyser::getAdjList(std::istream& in, int graphNum, bool errorCheck = true)
{
    int numVertices = getInt(in, "number of vertices in graph " + std::to_string(graphNum));

    MEDTester::Matrix adjList(numVertices, MEDTester::MatrixLine(3, -1));

    for (int u = 0; u < numVertices; ++u) {
        std::unordered_set<int> adjacentVertices;
        for (int i = 0; i < 3; i++) {
            int v = getInt(in, "adjacency list entry in graph " + std::to_string(graphNum));
            
            if (errorCheck) {
                if (v < 0 || v >= numVertices) {
                    throw WrongInputException(WRONG_INPUT_FORMAT_MESSAGE(
                        "vertex " + std::to_string(v) + " out of range, graph " + std::to_string(graphNum)
                    ));
                }
                if (v == u) {
                    throw WrongInputException(WRONG_INPUT_FORMAT_MESSAGE(
                        "vertex " + std::to_string(v) + " has a loop, graph " + std::to_string(graphNum)
                    ));
                }
                if (adjacentVertices.count(v)) {
                    throw WrongInputException(WRONG_INPUT_FORMAT_MESSAGE(
                        "double edge from vertex " + std::to_string(u) + " to vertex " + std::to_string(v) + ", graph " + std::to_string(graphNum)
                    ));
                }
                if (adjList[v][0] != -1 && adjList[v][0] != u && adjList[v][1] != u && adjList[v][2] != u) {
                    throw WrongInputException(WRONG_INPUT_FORMAT_MESSAGE(
                        "trying to add edge from vertex " + std::to_string(u) + " to vertex " + std::to_string(v) + " while vertex " + std::to_string(v) + " has already 3 edges to other vertices, graph " + std::to_string(graphNum)
                    ));
                }
            }

            adjList[u][i] = v;
            adjacentVertices.insert(v);
        }
    }

    return adjList;
}
