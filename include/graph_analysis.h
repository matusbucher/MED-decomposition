#include <OutputMode.h>

#include <iostream>

#ifndef GRAPH_ANALYSIS_H
#define GRAPH_ANALYSIS_H

namespace graph_analysis
{

void analyzeGraphs(std::istream& in, std::ostream& out, OutputMode mode, bool showTime);

void onlyResultMode(std::istream& in, std::ostream& out);

void notColorableMode(std::istream& in, std::ostream& out);

void notColorableBridgelessMode(std::istream& in, std::ostream& out);

void coloringMode(std::istream& in, std::ostream& out);

}

#endif