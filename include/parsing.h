#include <OutputMode.h>

#include <string>
#include <fstream>

#ifndef PARSING_H
#define PARSING_H

namespace parsing
{

char* getOption(char** begin, char** end, const std::string& option);

bool optionExists(char** begin, char** end, const std::string& option);

void checkSyntax(char** begin, char** end);

bool getInputFile(char** begin, char** end, std::ifstream& file);

bool getOutputFile(char** begin, char** end, std::ofstream& file);

OutputMode getOutputMode(char** begin, char** end);

bool getTimeOption(char** begin, char** end);

}

#endif