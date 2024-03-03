#include <OutputMode.h>
#include <parsing.h>
#include <graph_analysis.h>

#include <minisat/core/Solver.h>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {

    using Minisat::mkLit;
    using Minisat::lbool;

    Minisat::Solver solver;
    // Create variables
    auto A = solver.newVar();
    auto B = solver.newVar();
    auto C = solver.newVar();
    
    // Create the clauses
    solver.addClause( mkLit(A),  mkLit(B),  mkLit(C));
    solver.addClause(~mkLit(A),  mkLit(B),  mkLit(C));
    solver.addClause( mkLit(A), ~mkLit(B),  mkLit(C));
    solver.addClause( mkLit(A),  mkLit(B), ~mkLit(C));
    
    // Check for solution and retrieve model if found
    auto sat = solver.solve();
    if (sat) {
        std::clog << "SAT\n"
                  << "Model found:\n";
        std::clog << "A := " << (solver.modelValue(A) == l_True) << '\n';
        std::clog << "B := " << (solver.modelValue(B) == l_True) << '\n';
        std::clog << "C := " << (solver.modelValue(C) == l_True) << '\n';
    } else {
        std::clog << "UNSAT\n";
        return 1;
    }

    // char** begin = argv + 1;
    // char** end = argv + argc;

    // parsing::checkSyntax(begin, end);

    // std::ifstream inFile;
    // std::istream& in = parsing::getInputFile(begin, end, inFile) ? inFile : std::cin;

    // std::ofstream outFile;
    // std::ostream& out = parsing::getOutputFile(begin, end, outFile) ? outFile : std::cout;

    // OutputMode mode = parsing::getOutputMode(begin, end);
    // bool showTime = parsing::getTimeOption(begin, end);

    // graph_analysis::analyzeGraphs(in, out, mode, showTime);

    return 0;
}
