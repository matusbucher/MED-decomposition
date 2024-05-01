#include "GraphAnalyser.h"
#include "Parser.h"

#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    char** begin = argv + 1;
    char** end = argv + argc;

    try {
        MEDTester::Parser parser(begin, end);
        MEDTester::GraphAnalyser analyser(parser);
        analyser.analyze();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
