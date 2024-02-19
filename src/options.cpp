#include <options.h>
#include <messages.h>
#include <output_modes.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <algorithm>


char* getOption(char** begin, char** end, const std::string& option) {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) return *itr;
    return nullptr;
}

bool optionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}


void checkSyntax(char** begin, char** end) {
    bool inputOption, outputOption, modeOption, timeOption = false;

    for (char** itr = begin; itr < end; itr++) {
        if (strcmp(*itr, "help") == 0) {
            std::cout << HELP_MESSAGE;
            exit(0);
        }

        if (INPUT_OPTION_SPECIFIERS.count(*itr)) {
            if (inputOption) {
                std::cerr << doubleOptionMessage("input");
                exit(1);
            }
            inputOption = true;
            itr++;
        }
        else if (OUTPUT_OPTION_SPECIFIERS.count(*itr)) {
            if (outputOption) {
                std::cerr << doubleOptionMessage("output");
                exit(1);
            }
            outputOption = true;
            itr++;
        }
        else if (MODE_OPTION_SPECIFIERS.count(*itr)) {
            if (modeOption) {
                std::cerr << doubleOptionMessage("input");
                exit(1);
            }
            modeOption = true;
            itr++;
        }
        else if (TIME_OPTION_SPECIFIERS.count(*itr)) {
            if (timeOption) {
                std::cerr << doubleOptionMessage("time");
                exit(1);
            }
            timeOption = true;
        }
        else {
            std::cerr << invalidParameterMessage(*itr);
            exit(1);
        }
    }
}

bool getInputFile(char** begin, char** end, std::ifstream& file) {
    std::string specifier;
    for (std::string s : INPUT_OPTION_SPECIFIERS) {
        if (optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return false;

    char* filename = getOption(begin, end, specifier);
    if (filename == nullptr) {
        std::cerr << noFilenameMessage(specifier);
        exit(1);
    }
    if (!std::filesystem::exists(filename)) {
        std::cerr << fileNotFoundMessage(filename);
        exit(1);
    }

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << cannotOpenFileMessage(filename);
        exit(1);
    }

    return true;
}

bool getOutputFile(char** begin, char** end, std::ofstream& file) {
    std::string specifier;
    for (std::string s : OUTPUT_OPTION_SPECIFIERS) {
        if (optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return false;

    char* filename = getOption(begin, end, specifier);
    if (filename == nullptr) {
        std::cerr << noFilenameMessage(specifier);
        exit(1);
    }
    if (!std::filesystem::exists(filename)) {
        std::cerr << fileNotFoundMessage(filename);
        exit(1);
    }

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << cannotOpenFileMessage(filename);
        exit(2);
    }

    return true;
}

OutputMode getOutputMode(char** begin, char** end) {
    std::string specifier;
    for (std::string s : MODE_OPTION_SPECIFIERS) {
        if (optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return OutputMode::ONLY_RESULT;

    char* modename = getOption(begin, end, specifier);
    if (modename == nullptr) {
        std::cerr << noModenameMessage(specifier);
        exit(1);
    }

    if (ONLY_RESULT_MODE_SPECIFIERS.count(modename)) return OutputMode::ONLY_RESULT;
    if (NOT_COLORABLE_MODE_SPECIFIERS.count(modename)) return OutputMode::NOT_COLORABLE;
    if (NOT_COLORABLE_BRIDGELESS_MODE_SPECIFIERS.count(modename)) return OutputMode::NOT_COLORABLE_BRIDGELESS;
    if (COLORING_MODE_SPECIFIERS.count(modename)) return OutputMode::COLORING;

    std::cerr << invalidModeMessage(modename);
    exit(1);
}

bool getTimeOption(char** begin, char** end) {
    for (std::string s : TIME_OPTION_SPECIFIERS) {
        if (optionExists(begin, end, s)) return true;
    }
    return false;
}
