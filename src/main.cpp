#include "GraphAnalyser.h"
#include "Parser.h"

#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    char** begin = argv + 1;
    char** end = argv + argc;

    try {
        MedTester::Parser parser(begin, end);
        MedTester::GraphAnalyser analyser(parser);
        analyser.analyze();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
