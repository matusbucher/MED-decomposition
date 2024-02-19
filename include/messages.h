#include <string>

#ifndef MESSAGES_H
#define MESSAGES_H

static const std::string USE_HELP_MESSAGE = "Use parameter 'help' for usage description.\n";

static const std::string HELP_MESSAGE = "TODO\n";

static const std::string NOT_IMPLEMENTED_MODE_MESSAGE = "Error : Given output mode is not implemented yet.\n";

inline std::string doubleOptionMessage(std::string option) {
    return "Invalid syntax : Found two parameters of the same option '" + option + "'. Use only one of them.\n";
}

inline std::string invalidParameterMessage(std::string parameter) {
    return "Inavlid syntax : A parameter cannot be found that matches parameter '" + parameter + "'.\n" + USE_HELP_MESSAGE;
}

inline std::string noFilenameMessage(std::string specifier) {
    return "Invalid syntax : Found no filename after parameter '" + specifier + "'. Use parameter '" + specifier + "' with a path to file.\n";
}

inline std::string fileNotFoundMessage(std::string filename) {
    return "Error : File '" + filename + "' does not exist.\n";
}

inline std::string cannotOpenFileMessage(std::string filename) {
    return "Error : Cannot open file '" + filename + "'.\n";
}

inline std::string noModenameMessage(std::string specifier) {
    return "Invalid syntax : Found no mode specifier after parameter '" + specifier + "'.\n" + USE_HELP_MESSAGE;
}

inline std::string invalidModeMessage(std::string modename) {
    return "Invalid syntax : Mode specifier '" + modename + "' does not match any available modes.\n" + USE_HELP_MESSAGE;
}

#endif