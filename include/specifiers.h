#include <unordered_set>
#include <string>

#ifndef SPECIFIERS_H
#define SPECIFIERS_H

namespace specifiers
{

/* Specifies the input file / stream (a file name is expected after the specifier), stdin is default. */
static const std::unordered_set<std::string> INPUT_OPTION { "--input", "--in", "-i" };

/* Specifies the output file / stream (a file name is expected after the specifier), stdout is default. */
static const std::unordered_set<std::string> OUTPUT_OPTION { "--output", "--out", "-o" };

/* Specifies the output format mode, but some modes can also affect run of the program (a mode name is expected
   after the specifier), "only result" mode is default. */
static const std::unordered_set<std::string> MODE_OPTION { "--mode", "-m" };

/* Adds execution time at the end of output. */
static const std::unordered_set<std::string> TIME_OPTION { "--time", "-t" };

/* "Only result" mode outputs for each graph only whether the graph is or is not MED-colorable (yes/no answer). */
static const std::unordered_set<std::string> ONLY_RESULT_MODE { "onlyresult", "only-result", "result", "r", "or" };

/* "Not colorable" mode outputs numbers of graphs that are not MED-colorable. */
static const std::unordered_set<std::string> NOT_COLORABLE_MODE { "notcolorable", "not-colorable", "n", "nc" };

/* "Not colorable bridgeless" mode outputs numbers of graphs that are not MED-colorable and also bridgeless. */
static const std::unordered_set<std::string> NOT_COLORABLE_BRIDGELESS_MODE { "notcolorablebridgeless", "not-colorable-bridgeless", "b", "ncb" };

/* "Coloring" mode outputs for each graph concrete MED-coloring if the graph is MED-colorable, or no coloring
   if the graph is not MED-colorable. Coloring output format is the same as input format, but with colors of
   corresponding edges.
   Explanation of color representation:
      m := matching edges (subgraph G1 in MED decomposition)
      c := even cycle edges (subgraph G2 in MED decomposition)
      s := double-stars center edges (subgraph G3 - H in MED decomposition)
      h := double-stars point edges (subgraph H in MED decomposition) */
static const std::unordered_set<std::string> COLORING_MODE { "coloring", "c" };

}

#endif