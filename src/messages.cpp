#include <messages.h>
#include <specifiers.h>

#include <string>
#include <unordered_set>

inline std::string optionSpecifiersHelp(const std::unordered_set<std::string> specifiers, const std::string extraArgument = "") {
    std::string toReturn = "[";
    for (std::unordered_set<std::string>::const_iterator itr = specifiers.begin(); itr != specifiers.end(); itr++) {
        if (itr != specifiers.begin()) toReturn += " | ";
        toReturn += (*itr);
    }
    if (!extraArgument.empty()) toReturn += " " + extraArgument;
    toReturn += "]";
    return toReturn;
}

std::string helpMessage() {
    std::string toReturn = "usage: " + PROGRAM_NAME;
    toReturn += " " + optionSpecifiersHelp(INPUT_OPTION_SPECIFIERS, "<path>");
    toReturn += " " + optionSpecifiersHelp(OUTPUT_OPTION_SPECIFIERS, "<path>");
    toReturn += " " + optionSpecifiersHelp(MODE_OPTION_SPECIFIERS, "<mode_name>");
    toReturn += " " + optionSpecifiersHelp(TIME_OPTION_SPECIFIERS);
    toReturn += "\n";
    return toReturn;
}

std::string doubleOptionMessage(const std::string option) {
    return "Invalid syntax : Found two parameters of the same option '" + option + "'. Use only one of them.\n";
}

std::string invalidParameterMessage(const std::string parameter) {
    return "Inavlid syntax : A parameter cannot be found that matches parameter '" + parameter + "'.\n" + USE_HELP_MESSAGE;
}

std::string noFilenameMessage(const std::string specifier) {
    return "Invalid syntax : Found no filename after parameter '" + specifier + "'. Use parameter '" + specifier + "' with a path to file.\n";
}

std::string fileNotFoundMessage(const std::string filename) {
    return "Error : File '" + filename + "' does not exist.\n";
}

std::string cannotOpenFileMessage(const std::string filename) {
    return "Error : Cannot open file '" + filename + "'.\n";
}

std::string noModenameMessage(const std::string specifier) {
    return "Invalid syntax : Found no mode specifier after parameter '" + specifier + "'.\n" + USE_HELP_MESSAGE;
}

std::string invalidModeMessage(const std::string modename) {
    return "Invalid syntax : Mode specifier '" + modename + "' does not match any available modes.\n" + USE_HELP_MESSAGE;
}
