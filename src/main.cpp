#include <output_modes.h>
#include <parsing.h>
#include <graph_analysis.h>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {

    char** begin = argv + 1;
    char** end = argv + argc;

    checkSyntax(begin, end);

    std::ifstream inFile;
    std::istream& in = getInputFile(begin, end, inFile) ? inFile : std::cin;

    std::ofstream outFile;
    std::ostream& out = getOutputFile(begin, end, outFile) ? outFile : std::cout;

    OutputMode mode = getOutputMode(begin, end);
    bool showTime = getTimeOption(begin, end);

    analyzeGraphs(in, out, mode, showTime);

    return 0;
}
