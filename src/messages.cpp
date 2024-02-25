#include <messages.h>
#include <specifiers.h>

#include <string>
#include <unordered_set>

std::string listElementsFromSet(const std::unordered_set<std::string> set, const std::string separator) {
    std::string toReturn;
    for (std::unordered_set<std::string>::const_iterator itr = set.begin(); itr != set.end(); itr++) {
        if (itr != set.begin()) toReturn += separator;
        toReturn += (*itr);
    }
    return toReturn;
}

std::string helpMessage() {
    const std::string TAB = "   ";

    std::string toReturn = "MED is a program for testing cubic graphs - whether they are MED-decomposable or not.\n\n";

    toReturn += "USAGE\n";
    toReturn += TAB + PROGRAM_NAME;
    toReturn += " [" + listElementsFromSet(INPUT_OPTION_SPECIFIERS, " | ") + " <path>]";
    toReturn += " [" + listElementsFromSet(OUTPUT_OPTION_SPECIFIERS, " | ") + " <path>]";
    toReturn += " [" + listElementsFromSet(MODE_OPTION_SPECIFIERS, " | ") + " <mode_name>]";
    toReturn += " [" + listElementsFromSet(TIME_OPTION_SPECIFIERS, " | ") + "]";
    toReturn += "\n\n";

    toReturn += "OPTIONS\n";
    toReturn += TAB + listElementsFromSet(INPUT_OPTION_SPECIFIERS, ", ") + "\n";
    toReturn += TAB + TAB + "Specifies input file from which the program should read graphs. Default is stadnard input. For input format,\n";
    toReturn += TAB + TAB + "see 'INPUT FORMAT' section.\n";
    toReturn += TAB + listElementsFromSet(OUTPUT_OPTION_SPECIFIERS, ", ") + "\n";
    toReturn += TAB + TAB + "Specifies output file to which the program should write results. Default is standard output.\n";
    toReturn += TAB + listElementsFromSet(MODE_OPTION_SPECIFIERS, ", ") + "\n";
    toReturn += TAB + TAB + "Specifies formating of output. Use with one of the following mode names (default is 'only result' mode):\n";
    toReturn += TAB + TAB + listElementsFromSet(ONLY_RESULT_MODE_SPECIFIERS, " / ") + "\n";
    toReturn += TAB + TAB + TAB + "- Prints only the answer 'true' or 'false' ('true' = graph is MED-decomposable).\n";
    toReturn += TAB + TAB + listElementsFromSet(NOT_COLORABLE_MODE_SPECIFIERS, " / ") + "\n";
    toReturn += TAB + TAB + TAB + "- Prints only numbers of those graphs, that are not MED-decomposable.\n";
    toReturn += TAB + TAB + listElementsFromSet(NOT_COLORABLE_BRIDGELESS_MODE_SPECIFIERS, " / ") + "\n";
    toReturn += TAB + TAB + TAB + "- Prints only numbers of those graphs, that are not MED-decomposable and bridgeless.\n";
    toReturn += TAB + TAB + listElementsFromSet(COLORING_MODE_SPECIFIERS, " / ") + "\n";
    toReturn += TAB + TAB + TAB + "- Prints one of possible MED decomposition or 'false' if there is not such decomposition. Output format is\n";
    toReturn += TAB + TAB + TAB + "  similar to the input format, except 'colors' are added to each edge. 'm' is matching edge, 'c' is cycle\n";
    toReturn += TAB + TAB + TAB + "  edge, 's' is double-star center edge and 'h' is double-star point edge.\n";
    toReturn += TAB + listElementsFromSet(TIME_OPTION_SPECIFIERS, ", ") + "\n";
    toReturn += TAB + TAB + "Adds execution time at the end of the output.\n\n";

    toReturn += "INPUT FORMAT\n";
    toReturn += TAB + "The input should consist of only integers separated by any whitespace. The first integer is a total number of tested graphs.\n";
    toReturn += TAB + "Then follows that number of graph descriptions. A graph description starts with the number of graph (can be actually any number),\n";
    toReturn += TAB + "followed by the number of vertices. Vertices are by default numbered from 0 to n-1, where n is the number of vertices. Next n\n";
    toReturn += TAB + "\"lines\" represents the adjency list - each \"line\" consists of three integers. If k-th line has integers a, b and c, then\n";
    toReturn += TAB + "vertex k is adjacent to vertices a, b and c (it does not matter in which order are integers a, b, c). Similarly, the a-th \"line\"\n";
    toReturn += TAB + "should contain number k, otherwise reading the graph from input causes an error. Here is an example:\n";
    toReturn += TAB + TAB + "2\n";
    toReturn += TAB + TAB + "1\n";
    toReturn += TAB + TAB + "4\n";
    toReturn += TAB + TAB + "1 2 3\n";
    toReturn += TAB + TAB + "0 2 3\n";
    toReturn += TAB + TAB + "0 1 3\n";
    toReturn += TAB + TAB + "0 1 2\n";
    toReturn += TAB + TAB + "2\n";
    toReturn += TAB + TAB + "10\n";
    toReturn += TAB + TAB + "1 2 3\n";
    toReturn += TAB + TAB + "0 4 5\n";
    toReturn += TAB + TAB + "0 6 7\n";
    toReturn += TAB + TAB + "0 8 9\n";
    toReturn += TAB + TAB + "1 7 8\n";
    toReturn += TAB + TAB + "1 6 9\n";
    toReturn += TAB + TAB + "2 5 8\n";
    toReturn += TAB + TAB + "2 4 9\n";
    toReturn += TAB + TAB + "3 4 6\n";
    toReturn += TAB + TAB + "3 5 7\n";
    toReturn += TAB + "(graph number 1 is complete graph on 4 vertices, graph number 2 is the Petersen graph)\n";

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
