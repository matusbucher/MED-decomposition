#include <string>
#include <unordered_set>

#ifndef MESSAGES_H
#define MESSAGES_H

static const std::string PROGRAM_NAME = "med";

static const std::string USE_HELP_MESSAGE = "Use parameter 'help' for usage description.\n";

static const std::string NOT_IMPLEMENTED_MODE_MESSAGE = "Error : Given output mode is not implemented yet.\n";

std::string helpMessage();

std::string doubleOptionMessage(const std::string option);

std::string invalidParameterMessage(const std::string parameter);

std::string noFilenameMessage(const std::string specifier);

std::string fileNotFoundMessage(const std::string filename);

std::string cannotOpenFileMessage(const std::string filename);

std::string noModenameMessage(const std::string specifier);

std::string invalidModeMessage(const std::string modename);

#endif