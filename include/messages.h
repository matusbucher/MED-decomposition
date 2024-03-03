#include <string>
#include <unordered_set>

#ifndef MESSAGES_H
#define MESSAGES_H

namespace message
{

static const std::string PROGRAM_NAME = "med";

static const std::string USE_HELP = "Use parameter 'help' for usage description.\n";

static const std::string NOT_IMPLEMENTED_MODE = "Error : Given output mode is not implemented yet.\n";

std::string help();

std::string doubleOption(const std::string option);

std::string invalidParameter(const std::string parameter);

std::string noFilename(const std::string specifier);

std::string fileNotFound(const std::string filename);

std::string cannotOpenFile(const std::string filename);

std::string noModename(const std::string specifier);

std::string invalidMode(const std::string modename);

}

#endif