#ifndef MEDTESTER_UTILS_GRAPH_ANALYSER_H_
#define MEDTESTER_UTILS_GRAPH_ANALYSER_H_

#include "OutputMode.h"
#include "Parser.h"

#include "CubicGraph.h"

#include <string>
#include <iostream>

namespace MedTester
{

class GraphAnalyser
{
public:
    // Exception thrown when the graph input is in wrong format
    class WrongInputException : public std::exception
    {
        private:
            std::string message = "Wrong Input: ";

        public:
            WrongInputException(const std::string& msg) throw() { message += msg; }
            virtual ~WrongInputException() throw() {}
            const char* what() const throw() { return message.c_str(); }
    };

    // Exception thrown when an error with a file occurs
    class FileErrorException : public std::exception
    {
        private:
            std::string message = "File Error: ";

        public:
            FileErrorException(const std::string& msg) throw() { message += msg; }
            virtual ~FileErrorException() throw() {}
            const char* what() const throw() { return message.c_str(); }
    };

    // Constructors and deconstructor:
    GraphAnalyser(MedTester::Parser& parser);
    GraphAnalyser(const std::string& inputFilename, const std::string& outputFilename, MedTester::OutputMode outputMode, bool showTime);
    ~GraphAnalyser();

    // Standard analysis function
    void analyze() const;

private:
    // Options for graph analysis (set in constructor):
    std::string mInputFilename;
    std::string mOutputFilename;
    MedTester::OutputMode mOutputMode;
    bool mShowTime;

    // Analysis functions for each output mode:
    static void onlyResultMode(std::istream& in, std::ostream& out);
    static void notDecomposableMode(std::istream& in, std::ostream& out);
    static void notDecomposableBridgelessMode(std::istream& in, std::ostream& out);
    static void coloringMode(std::istream& in, std::ostream& out);

    // Private static constants, messages and utility functions:
    static inline std::string WRONG_INPUT_FORMAT_MESSAGE(const std::string& additionalInfo);
    static inline std::string INPUT_FILE_DOES_NOT_EXIST_MESSAGE(const std::string& filename);
    static inline std::string CANNOT_OPEN_FILE_MESSAGE(const std::string& filename);

    static int getInt(std::istream& in, const std::string& what);
    static std::vector<std::vector<int>> getAdjList(std::istream& in, int graphNum, bool errorCheck);
};

} // namespace MedTester

#endif // MEDTESTER_UTILS_GRAPH_ANALYSER_H_
