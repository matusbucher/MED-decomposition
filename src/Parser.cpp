#include "Parser.h"

#include "OutputMode.h"

#include <string>
#include <cstring>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <stdexcept>


const MedTester::Parser::OptionInfo<std::string> MedTester::Parser::INPUT_FILENAME_OPTION_INFO
{
    "input filename",
    "Specifies input file from which the program should read graphs. If omitted, standard input is used.",
    { "--input", "-i" },
    true,
    std::string()
};

const MedTester::Parser::OptionInfo<std::string> MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO
{
    "output filename",
    "Specifies output file to which the program should write results. If omitted, standard output is used.",
    { "--output", "-o" },
    true,
    std::string()
};

const MedTester::Parser::OptionInfo<MedTester::OutputMode> MedTester::Parser::OUTPUT_MODE_OPTION_INFO
{
    "output mode",
    "Specifies formating of output.",
    { "--mode", "-m" },
    true,
    MedTester::OutputMode::ONLY_RESULT
};

const MedTester::Parser::OptionInfo<bool> MedTester::Parser::SHOW_TIME_OPTION_INFO
{
    "show time",
    "Adds execution time at the end of the output.",
    { "--time", "-t" },
    false,
    false
};

const MedTester::Parser::ModeInfo<MedTester::OutputMode> MedTester::Parser::ONLY_RESULT_MODE_INFO
{
    "only result",
    "Prints only the answer 'true' or 'false' ('true' = graph is MED-decomposable).",
    { "onlyresult", "or", "r" },
    MedTester::OutputMode::ONLY_RESULT
};

const MedTester::Parser::ModeInfo<MedTester::OutputMode> MedTester::Parser::NOT_DECOMPOSABLE_MODE_INFO
{
    "not decomposable",
    "Prints only numbers of those graphs, that are not MED-decomposable.",
    { "notdecomposable", "nd", "d" },
    MedTester::OutputMode::NOT_DECOMPOSABLE
};

const MedTester::Parser::ModeInfo<MedTester::OutputMode> MedTester::Parser::NOT_DECOMPOSABLE_BRIDGELESS_MODE_INFO
{
    "not decomposable bridgeless",
    "Prints only numbers of those graphs, that are not MED-decomposable and bridgeless.",
    { "notdecomposablebridgeless", "ndb", "b" },
    MedTester::OutputMode::NOT_DECOMPOSABLE_BRIDGELESS
};

const MedTester::Parser::ModeInfo<MedTester::OutputMode> MedTester::Parser::COLORING_MODE_INFO
{
    "coloring",
    "Prints one of possible MED decomposition or 'false' if there is not such decomposition. Output format is similar to the input format, except 'colors' are added to each edge. 'm' is matching edge, 'c' is cycle edge, 's' is double-star center edge and 'h' is double-star point edge.",
    { "coloring", "c" },
    MedTester::OutputMode::COLORING
};

const std::vector<MedTester::Parser::ModeInfo<MedTester::OutputMode>> MedTester::Parser::AVAILABLE_OUTPUTMODES
{
    MedTester::Parser::ONLY_RESULT_MODE_INFO,
    MedTester::Parser::NOT_DECOMPOSABLE_MODE_INFO,
    MedTester::Parser::NOT_DECOMPOSABLE_BRIDGELESS_MODE_INFO,
    MedTester::Parser::COLORING_MODE_INFO

};

const std::string MedTester::Parser::HELP_SPECIFIER = "help";


MedTester::Parser::Parser(char** begin, char** end)
{
    if (begin == nullptr || end == nullptr || begin > end)
        throw std::invalid_argument(INVALID_ARGUMENT_RANGE_MESSAGE);
    
    mBegin = begin;
    mEnd = end;
    mInputFilename = INPUT_FILENAME_OPTION_INFO.defaultValue;
    mOutputFilename = OUTPUT_FILENAME_OPTION_INFO.defaultValue;
    mOutputMode = OUTPUT_MODE_OPTION_INFO.defaultValue;
    mShowTime = SHOW_TIME_OPTION_INFO.defaultValue;
}

MedTester::Parser::~Parser() {}


char** MedTester::Parser::getBegin() const
{
    return mBegin;
}

char** MedTester::Parser::getEnd() const
{
    return mEnd;
}

std::string MedTester::Parser::getInputFilename() const
{
    return mInputFilename;
}

std::string MedTester::Parser::getOutputFilename() const
{
    return mOutputFilename;
}

MedTester::OutputMode MedTester::Parser::getOutputMode() const
{
    return mOutputMode;
}

bool MedTester::Parser::getShowTime() const
{
    return mShowTime;
}

void MedTester::Parser::setArgumentRange(char** begin, char** end)
{
    if (begin == nullptr || end == nullptr || begin > end)
        throw std::invalid_argument(INVALID_ARGUMENT_RANGE_MESSAGE);

    mBegin = begin;
    mEnd = end;
}

void MedTester::Parser::setInputFilename(const std::string& filename)
{
    mInputFilename = filename;
}

void MedTester::Parser::setOutputFilename(const std::string& filename)
{
    mOutputFilename = filename;
}

void MedTester::Parser::setOutputMode(MedTester::OutputMode outputMode)
{
    mOutputMode = outputMode;
}

void MedTester::Parser::setShowTime(bool showTime)
{
    mShowTime = showTime;
}


void MedTester::Parser::checkSyntax(char** begin, char** end)
{
    if (begin == nullptr || end == nullptr || begin > end)
        throw std::invalid_argument(INVALID_ARGUMENT_RANGE_MESSAGE);

    if (begin == end) return;

    // If found 'help option' at the beginning, output help message and terminate
    if (strcmp(begin[0], MedTester::Parser::HELP_SPECIFIER.c_str()) == 0) {
        MedTester::Parser::printHelpMessage();
        exit(0);
    }

    std::unordered_set<std::string> mentionedOptions;

    for (char** it = begin; it != end; ++it) {
        std::string optionName;
        bool optionHasArg;

        if (std::find(MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers.begin(), MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers.end(), *it) != MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers.end()) {
            optionName = INPUT_FILENAME_OPTION_INFO.name;
            optionHasArg = INPUT_FILENAME_OPTION_INFO.hasArg;
        }
        else if (std::find(MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers.begin(), MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers.end(), *it) != MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers.end()) {
            optionName = OUTPUT_FILENAME_OPTION_INFO.name;
            optionHasArg = OUTPUT_FILENAME_OPTION_INFO.hasArg;
        }
        else if (std::find(MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers.begin(), MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers.end(), *it) != MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers.end()) {
            optionName = OUTPUT_MODE_OPTION_INFO.name;
            optionHasArg = OUTPUT_MODE_OPTION_INFO.hasArg;
        }
        else if (std::find(MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers.begin(), MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers.end(), *it) != MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers.end()) {
            optionName = SHOW_TIME_OPTION_INFO.name;
            optionHasArg = SHOW_TIME_OPTION_INFO.hasArg;
        }
        else throw MedTester::Parser::InvalidSyntaxException(MedTester::Parser::UNKNOWN_OPTION_MESSAGE(*it));

        if (mentionedOptions.count(optionName))
            throw MedTester::Parser::InvalidSyntaxException(MedTester::Parser::DOUBLE_OPTION_MESSAGE(optionName));

        if (optionHasArg && ++it == end)
            throw MedTester::Parser::InvalidSyntaxException(MedTester::Parser::EXPECTED_OPTION_ARGUMENT_MESSAGE(optionName));

        mentionedOptions.insert(optionName);
    }
}

bool MedTester::Parser::optionExists(char** begin, char** end, const std::string& option)
{
    if (begin == nullptr || end == nullptr || begin > end)
        throw std::invalid_argument(INVALID_ARGUMENT_RANGE_MESSAGE);

    return std::find(begin, end, option) != end;
}

std::string MedTester::Parser::getOptionArgument(char** begin, char** end, const std::string& option)
{
    if (begin == nullptr || end == nullptr || begin > end)
        throw std::invalid_argument(INVALID_ARGUMENT_RANGE_MESSAGE);

    char** arg = std::find(begin, end, option);

    if (arg == end || ++arg == end)
        throw MedTester::Parser::InvalidSyntaxException(MedTester::Parser::EXPECTED_OPTION_ARGUMENT_MESSAGE(option));

    return std::string(*arg);
}

void MedTester::Parser::checkSyntax() const
{
    MedTester::Parser::checkSyntax(mBegin, mEnd);
}

bool MedTester::Parser::optionExists(const std::string& option) const
{
    return MedTester::Parser::optionExists(mBegin, mEnd, option);
}

std::string MedTester::Parser::getOptionArgument(const std::string& option) const
{
    return MedTester::Parser::getOptionArgument(mBegin, mEnd, option);
}


bool MedTester::Parser::parseInputFilename()
{
    std::string specifier;
    for (std::string s : MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers) {
        if (MedTester::Parser::optionExists(s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty())
        return false;
    
    std::string filename = MedTester::Parser::getOptionArgument(specifier);
    mInputFilename = filename;
    return true;
}

bool MedTester::Parser::parseOutputFilename()
{
    std::string specifier;
    for (std::string s : MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers) {
        if (MedTester::Parser::optionExists(s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty())
        return false;
    
    std::string filename = MedTester::Parser::getOptionArgument(specifier);
    mOutputFilename = filename;
    return true;
}

bool MedTester::Parser::parseOutputMode()
{
    std::string specifier;
    for (std::string s : MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers) {
        if (MedTester::Parser::optionExists(s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty())
        return false;

    std::string modeSpecifier = MedTester::Parser::getOptionArgument(specifier);
    for (MedTester::Parser::ModeInfo<MedTester::OutputMode> mode : MedTester::Parser::AVAILABLE_OUTPUTMODES) {
        if (std::find(mode.specifiers.begin(), mode.specifiers.end(), modeSpecifier) != mode.specifiers.end()) {
            mOutputMode = mode.value;
            return true;
        }
    }

    throw MedTester::Parser::InvalidSyntaxException(MedTester::Parser::UNKNOWN_OUTPUT_MODE_MESSAGE(modeSpecifier));
}

bool MedTester::Parser::parseShowTime()
{
    for (std::string s : MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers) {
        if (MedTester::Parser::optionExists(s)) {
            mShowTime = true;
            return true;
        }
    }

    return false;
}

void MedTester::Parser::parseAll()
{
    MedTester::Parser::parseInputFilename();
    MedTester::Parser::parseOutputFilename();
    MedTester::Parser::parseOutputMode();
    MedTester::Parser::parseShowTime();
}


void MedTester::Parser::printHelpMessage()
{
    size_t TAB = 3;
    size_t WIDTH = 100;

    std::cout << "MED is a program for testing cubic graphs - whether they are MED-decomposable or not.\n\n";

    std::cout << "USAGE\n";
    std::cout << MedTester::Parser::formatText(
        "med" +
        MedTester::Parser::joinToString(MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers, " [", " <path>]", " | ") +
        MedTester::Parser::joinToString(MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers, " [", " <path>]", " | ") +
        MedTester::Parser::joinToString(MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers, " [", " <mode_name>]", " | ") +
        MedTester::Parser::joinToString(MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers, " [", "]", " | "),
        1*TAB,
        WIDTH
    ) << "\n";

    std::cout << "OPTIONS\n";
    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::joinToString(MedTester::Parser::INPUT_FILENAME_OPTION_INFO.specifiers, "", "", ", "),
        1*TAB,
        WIDTH
    );
    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::INPUT_FILENAME_OPTION_INFO.description,
        2*TAB,
        WIDTH
    ) << "\n";

    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::joinToString(MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.specifiers, "", "", ", "),
        1*TAB,
        WIDTH
    );
    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::OUTPUT_FILENAME_OPTION_INFO.description,
        2*TAB,
        WIDTH
    ) << "\n";

    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::joinToString(MedTester::Parser::OUTPUT_MODE_OPTION_INFO.specifiers, "", "", ", "),
        1*TAB,
        WIDTH
    );
    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::OUTPUT_MODE_OPTION_INFO.description +
        " Use with one of the following mode names (default is 'only result' mode):",
        2*TAB,
        WIDTH
    ) << "\n";

    for (MedTester::Parser::ModeInfo mode : MedTester::Parser::AVAILABLE_OUTPUTMODES) {
        std::cout << MedTester::Parser::formatText(MedTester::Parser::joinToString(mode.specifiers, "", "", " / "), 2*TAB, WIDTH);
        std::cout << MedTester::Parser::formatText(mode.description, 3*TAB, WIDTH) << "\n";
    }
    std::cout << "\n";

    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::joinToString(MedTester::Parser::SHOW_TIME_OPTION_INFO.specifiers, "", "", ", "),
        1*TAB,
        WIDTH
    );
    std::cout << MedTester::Parser::formatText(
        MedTester::Parser::SHOW_TIME_OPTION_INFO.description ,
        2*TAB,
        WIDTH
    ) << "\n";

    std::cout << "INPUT FORMAT\n";
    std::cout << MedTester::Parser::formatText(std::string() +
        "The input should consist of only integers separated by any whitespace. The first integer is a total number of tested graphs. " +
        "Then follows that number of graph descriptions. A graph description starts with the number of graph (can be actually any number), " +
        "followed by the number of vertices. Vertices are by default numbered from 0 to n-1, where n is the number of vertices. Next n " +
        "\"lines\" represents the adjency list - each \"line\" consists of three integers. If k-th line has integers a, b and c, then " +
        "vertex k is adjacent to vertices a, b and c (it does not matter in which order are integers a, b, c). Similarly, the a-th \"line\" " +
        "should contain number k, otherwise reading the graph from input causes an error. Here is an example:",
        1*TAB,
        WIDTH
    ) << "\n";

    std::cout << "\t2\n\t1\n\t4\n\t1 2 3\n\t0 2 3\n\t0 1 3\n\t0 1 2\n\t2\n\t10\n\t1 2 3\n\t0 4 5\n\t0 6 7\n\t0 8 9\n\t1 7 8\n\t1 6 9\n\t2 5 8\n\t2 4 9\n\t3 4 6\n\t3 5 7\n";
}


const std::string MedTester::Parser::INVALID_ARGUMENT_RANGE_MESSAGE = "Invalid pointers to an array";

inline std::string MedTester::Parser::UNKNOWN_OPTION_MESSAGE(const std::string& option)
{
    return "Uknown option '" + option + "'. Use '" + MedTester::Parser::HELP_SPECIFIER + "' for command description.";
}

inline std::string MedTester::Parser::DOUBLE_OPTION_MESSAGE(const std::string& option)
{
    return "Found two specifiers of the same option '" + option + "'. Use only one of them.";
}

inline std::string MedTester::Parser::EXPECTED_OPTION_ARGUMENT_MESSAGE(const std::string& option)
{
    return "Expected argument after option '" + option + "', but found none.";
}

inline std::string MedTester::Parser::UNKNOWN_OUTPUT_MODE_MESSAGE(const std::string& mode)
{
    return "Uknown output mode '" + mode + "'. Use '" + MedTester::Parser::HELP_SPECIFIER + "' for command description.";
}

std::string MedTester::Parser::formatText(const std::string& text, size_t indent, size_t width)
{
    size_t curPos = 0;
    size_t nextPos = 0;
    std::string toReturn;
    std::string substr = text.substr(curPos, width + 1 - indent);

    while (substr.length() == width + 1 - indent && (nextPos = substr.rfind(' ')) != text.npos) {
        toReturn += std::string(indent, ' ') + text.substr(curPos, nextPos) + "\n";
        curPos += nextPos + 1;
        substr = text.substr(curPos, width + 1 - indent);
    }
    if (curPos != text.length())
        toReturn += std::string(indent, ' ') + text.substr(curPos, text.npos) + "\n";

    return toReturn;
}

std::string MedTester::Parser::joinToString(std::vector<std::string> list, const std::string& prefix, const std::string& suffix, const std::string& separator)
{
    std::string toReturn = prefix;
    if (!list.empty()) {
        std::vector<std::string>::iterator it = list.begin();
        toReturn += *it;
        while (++it != list.end()) {
            toReturn += separator + *it;
        }
    }
    toReturn += suffix;

    return toReturn;
}
