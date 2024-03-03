#include <parsing.h>
#include <specifiers.h>
#include <messages.h>
#include <OutputMode.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <algorithm>


char* parsing::getOption(char** begin, char** end, const std::string& option) {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) return *itr;
    return nullptr;
}

bool parsing::optionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}


void parsing::checkSyntax(char** begin, char** end) {
    bool inputOption, outputOption, modeOption, timeOption = false;

    for (char** itr = begin; itr < end; itr++) {
        if (strcmp(*itr, "help") == 0) {
            std::cout << message::help();
            exit(0);
        }

        if (specifiers::INPUT_OPTION.count(*itr)) {
            if (inputOption) {
                std::cerr << message::doubleOption("input");
                exit(1);
            }
            inputOption = true;
            itr++;
        }
        else if (specifiers::OUTPUT_OPTION.count(*itr)) {
            if (outputOption) {
                std::cerr << message::doubleOption("output");
                exit(1);
            }
            outputOption = true;
            itr++;
        }
        else if (specifiers::MODE_OPTION.count(*itr)) {
            if (modeOption) {
                std::cerr << message::doubleOption("input");
                exit(1);
            }
            modeOption = true;
            itr++;
        }
        else if (specifiers::TIME_OPTION.count(*itr)) {
            if (timeOption) {
                std::cerr << message::doubleOption("time");
                exit(1);
            }
            timeOption = true;
        }
        else {
            std::cerr << message::invalidParameter(*itr);
            exit(1);
        }
    }
}

bool parsing::getInputFile(char** begin, char** end, std::ifstream& file) {
    std::string specifier;
    for (std::string s : specifiers::INPUT_OPTION) {
        if (parsing::optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return false;

    char* filename = parsing::getOption(begin, end, specifier);
    if (filename == nullptr) {
        std::cerr << message::noFilename(specifier);
        exit(1);
    }
    if (!std::filesystem::exists(filename)) {
        std::cerr << message::fileNotFound(filename);
        exit(1);
    }

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << message::cannotOpenFile(filename);
        exit(1);
    }

    return true;
}

bool parsing::getOutputFile(char** begin, char** end, std::ofstream& file) {
    std::string specifier;
    for (std::string s : specifiers::OUTPUT_OPTION) {
        if (parsing::optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return false;

    char* filename = parsing::getOption(begin, end, specifier);
    if (filename == nullptr) {
        std::cerr << message::noFilename(specifier);
        exit(1);
    }
    if (!std::filesystem::exists(filename)) {
        std::cerr << message::fileNotFound(filename);
        exit(1);
    }

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << message::cannotOpenFile(filename);
        exit(2);
    }

    return true;
}

OutputMode parsing::getOutputMode(char** begin, char** end) {
    std::string specifier;
    for (std::string s : specifiers::MODE_OPTION) {
        if (parsing::optionExists(begin, end, s)) {
            specifier = s;
            break;
        }
    }

    if (specifier.empty()) return OutputMode::ONLY_RESULT;

    char* modename = parsing::getOption(begin, end, specifier);
    if (modename == nullptr) {
        std::cerr << message::noModename(specifier);
        exit(1);
    }

    if (specifiers::ONLY_RESULT_MODE.count(modename)) return OutputMode::ONLY_RESULT;
    if (specifiers::NOT_COLORABLE_MODE.count(modename)) return OutputMode::NOT_COLORABLE;
    if (specifiers::NOT_COLORABLE_BRIDGELESS_MODE.count(modename)) return OutputMode::NOT_COLORABLE_BRIDGELESS;
    if (specifiers::COLORING_MODE.count(modename)) return OutputMode::COLORING;

    std::cerr << message::invalidMode(modename);
    exit(1);
}

bool parsing::getTimeOption(char** begin, char** end) {
    for (std::string s : specifiers::TIME_OPTION) {
        if (parsing::optionExists(begin, end, s)) return true;
    }
    return false;
}
