#include "GraphAnalyser.h"

#include "OutputMode.h"
#include "Parser.h"

#include "CubicGraph.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>


MedTester::GraphAnalyser::GraphAnalyser(MedTester::Parser& parser)
{
    parser.checkSyntax();
    parser.parseAll();

    mInputFilename = parser.getInputFilename();
    mOutputFilename = parser.getOutputFilename();
    mOutputMode = parser.getOutputMode();
    mShowTime = parser.getShowTime();
}

MedTester::GraphAnalyser::GraphAnalyser(const std::string& inputFilename, const std::string& outputFilename, MedTester::OutputMode outputMode, bool showTime)
{
    mInputFilename = inputFilename;
    mOutputFilename = outputFilename;
    mOutputMode = outputMode;
    mShowTime = showTime;
}

MedTester::GraphAnalyser::~GraphAnalyser() {}


void MedTester::GraphAnalyser::analyze() const
{
    std::istream *in;
    std::ifstream inputFile;
    if (mInputFilename == MedTester::Parser::INPUT_FILENAME_OPTION_INFO.defaultValue) {
        in = &std::cin;
    } else {
        if (!std::filesystem::exists(mInputFilename)) {
            throw MedTester::GraphAnalyser::FileErrorException(MedTester::GraphAnalyser::INPUT_FILE_DOES_NOT_EXIST_MESSAGE(mInputFilename));
        }
        inputFile.open(mInputFilename, std::ifstream::in);
        if (!inputFile.good()) {
            throw MedTester::GraphAnalyser::FileErrorException(MedTester::GraphAnalyser::CANNOT_OPEN_FILE_MESSAGE(mInputFilename));
        }
        in = &inputFile;
    }

    std::ostream *out;
    std::ofstream outputFile;
    if (mOutputFilename == MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.defaultValue) {
        out = &std::cout;
    } else {
        outputFile.open(mOutputFilename, std::ofstream::out);
        if (!outputFile.is_open())
            throw MedTester::GraphAnalyser::FileErrorException(MedTester::GraphAnalyser::CANNOT_OPEN_FILE_MESSAGE(mOutputFilename));
        out = &outputFile;
    }

    auto timeStart = std::chrono::high_resolution_clock::now();

    switch (mOutputMode)
    {
    case MedTester::OutputMode::ONLY_RESULT: 
        MedTester::GraphAnalyser::onlyResultMode(*in, *out);
        break;

    case MedTester::OutputMode::NOT_DECOMPOSABLE: 
        MedTester::GraphAnalyser::notDecomposableMode(*in, *out);
        break;

    case MedTester::OutputMode::NOT_DECOMPOSABLE_BRIDGELESS: 
        MedTester::GraphAnalyser::notDecomposableBridgelessMode(*in, *out);
        break;

    case MedTester::OutputMode::COLORING: 
        MedTester::GraphAnalyser::coloringMode(*in, *out);
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


void MedTester::GraphAnalyser::onlyResultMode(std::istream& in, std::ostream& out)
{
    int graphCount = MedTester::GraphAnalyser::getInt(in, "number of graphs");
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = MedTester::GraphAnalyser::getInt(in, "graph number, " + std::to_string(i) + ". graph");
        CubicGraph graph(MedTester::GraphAnalyser::getAdjList(in, graphNum, true), false);
        out << graphNum << ": " << (graph.isMedDecomposable() ? "true" : "false") << "\n";
    }
}

void MedTester::GraphAnalyser::notDecomposableMode(std::istream& in, std::ostream& out)
{
    int graphCount = MedTester::GraphAnalyser::getInt(in, "number of graphs");
    out << "Not decomposable graphs:\n";
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = MedTester::GraphAnalyser::getInt(in, "graph number, " + std::to_string(i) + ". graph");
        CubicGraph graph(MedTester::GraphAnalyser::getAdjList(in, graphNum, true), false);
        if (!graph.isMedDecomposable()) out << graphNum << "\n";
    }
}

void MedTester::GraphAnalyser::notDecomposableBridgelessMode(std::istream& in, std::ostream& out)
{
    int graphCount = MedTester::GraphAnalyser::getInt(in, "number of graphs");
    out << "Not decomposable and bridgeless graphs:\n";
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = MedTester::GraphAnalyser::getInt(in, "graph number, " + std::to_string(i) + ". graph");
        CubicGraph graph(MedTester::GraphAnalyser::getAdjList(in, graphNum, true), false);
        if (graph.isBridgeless() && !graph.isMedDecomposable()) out << graphNum << "\n";
    }
}

void MedTester::GraphAnalyser::coloringMode(std::istream& in, std::ostream& out)
{
    const std::unordered_map<CubicGraph::EdgeColor, std::string> edgeColorChar = {
        {CubicGraph::EdgeColor::NONE, "-"},
        {CubicGraph::EdgeColor::MATCHING, "m"},
        {CubicGraph::EdgeColor::CYCLE, "c"},
        {CubicGraph::EdgeColor::STAR_POINT, "h"},
        {CubicGraph::EdgeColor::STAR_CENTER, "s"}
    };

    int graphCount = MedTester::GraphAnalyser::getInt(in, "number of graphs");
    for (int i = 1; i <= graphCount; ++i) {
        int graphNum = MedTester::GraphAnalyser::getInt(in, "graph number, " + std::to_string(i) + ". graph");
        std::vector<std::vector<int>> adjList = MedTester::GraphAnalyser::getAdjList(in, graphNum, true);
        CubicGraph graph(adjList, false);
        out << "graph " << graphNum << ":\n";
        if (graph.isMedDecomposable()) {
            std::vector<std::vector<CubicGraph::EdgeColor>> coloring = graph.getMedColoring();
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
    }
}


inline std::string MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(const std::string& additionalInfo)
{
    return "Wrong input format (" + additionalInfo + "). Use '" + MedTester::Parser::HELP_SPECIFIER + "' for command description (see 'Input Format').";
}

inline std::string MedTester::GraphAnalyser::INPUT_FILE_DOES_NOT_EXIST_MESSAGE(const std::string& filename)
{
    return "Specified input file '" + filename + "' does not exist.";
}

inline std::string MedTester::GraphAnalyser::CANNOT_OPEN_FILE_MESSAGE(const std::string& filename)
{
    return "Cannot open specified file '" + filename + "'.";
}

int MedTester::GraphAnalyser::getInt(std::istream& in, const std::string& what)
{
    int x;
    if (!(in >> x)) {
        throw MedTester::GraphAnalyser::WrongInputException(MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE("missing " + what));
    }
    return x;
}

std::vector<std::vector<int>> MedTester::GraphAnalyser::getAdjList(std::istream& in, int graphNum, bool errorCheck = true)
{
    int numVertices = MedTester::GraphAnalyser::getInt(in, "number of vertices in graph " + std::to_string(graphNum));

    std::vector<std::vector<int>> adjList(numVertices, std::vector<int>(3, -1));

    for (int u = 0; u < numVertices; ++u) {
        std::unordered_set<int> adjacentVertices;
        for (int i = 0; i < 3; i++) {
            int v = MedTester::GraphAnalyser::getInt(in, "adjacency list entry in graph " + std::to_string(graphNum));
            
            if (errorCheck) {
                if (v < 0 || v >= numVertices) {
                    throw MedTester::GraphAnalyser::WrongInputException(MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(
                        "vertex " + std::to_string(v) + " out of range, graph " + std::to_string(graphNum)
                    ));
                }
                if (v == u) {
                    throw MedTester::GraphAnalyser::WrongInputException(MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(
                        "vertex " + std::to_string(v) + " has a loop, graph " + std::to_string(graphNum)
                    ));
                }
                if (adjacentVertices.count(v)) {
                    throw MedTester::GraphAnalyser::WrongInputException(MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(
                        "double edge from vertex " + std::to_string(u) + " to vertex " + std::to_string(v) + ", graph " + std::to_string(graphNum)
                    ));
                }
                if (adjList[v][0] != -1 && adjList[v][0] != u && adjList[v][1] != u && adjList[v][2] != u) {
                    throw MedTester::GraphAnalyser::WrongInputException(MedTester::GraphAnalyser::WRONG_INPUT_FORMAT_MESSAGE(
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
