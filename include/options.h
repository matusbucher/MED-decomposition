#include <output_modes.h>

#include <unordered_set>
#include <string>
#include <fstream>

#ifndef OPTIONS_H
#define OPTIONS_H

/* Specifies the input file / stream (a file name is expected after the specifier), stdin is default. */
const std::unordered_set<std::string> INPUT_OPTION_SPECIFIERS { "--input", "-in", "-i" };

/* Specifies the output file / stream (a file name is expected after the specifier), stdout is default. */
const std::unordered_set<std::string> OUTPUT_OPTION_SPECIFIERS { "--output", "-out", "-o" };

/* Specifies the output format mode, but some modes can also affect run of the program (a mode name is expected
   after the specifier), "only result" mode is default. */
const std::unordered_set<std::string> MODE_OPTION_SPECIFIERS { "--mode", "-m" };

/* Adds execution time at the end of output. */
const std::unordered_set<std::string> TIME_OPTION_SPECIFIERS { "--time", "-t" };

/* "Only result" mode outputs for each graph only whether the graph is or is not MED-colorable (yes/no answer). */
const std::unordered_set<std::string> ONLY_RESULT_MODE_SPECIFIERS { "onlyresult", "only-result", "result", "r", "or" };

/* "Not colorable" mode outputs numbers of graphs that are not MED-colorable. */
const std::unordered_set<std::string> NOT_COLORABLE_MODE_SPECIFIERS { "notcolorable", "not-colorable", "n", "nc" };

/* "Not colorable bridgeless" mode outputs numbers of graphs that are not MED-colorable and also bridgeless. */
const std::unordered_set<std::string> NOT_COLORABLE_BRIDGELESS_MODE_SPECIFIERS { "notcolorablebridgeless", "not-colorable-bridgeless", "b", "ncb" };

/* "Coloring" mode outputs for each graph concrete MED-coloring if the graph is MED-colorable, or no coloring
   if the graph is not MED-colorable. Coloring output format is the same as input format, but with colors of
   corresponding edges.
   Explanation of color representation:
      m := matching edges (subgraph G1 in MED decomposition)
      c := even cycle edges (subgraph G2 in MED decomposition)
      s := double-stars center edges (subgraph G3 - H in MED decomposition)
      h := double-stars point edges (subgraph H in MED decomposition) */
const std::unordered_set<std::string> COLORING_MODE_SPECIFIERS { "coloring", "c" };

char* getOption(char** begin, char** end, const std::string& option);

bool optionExists(char** begin, char** end, const std::string& option);

void checkSyntax(char** begin, char** end);

bool getInputFile(char** begin, char** end, std::ifstream& file);

bool getOutputFile(char** begin, char** end, std::ofstream& file);

OutputMode getOutputMode(char** begin, char** end);

bool getTimeOption(char** begin, char** end);

#endif