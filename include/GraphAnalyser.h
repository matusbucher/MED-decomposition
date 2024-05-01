#ifndef MEDTESTER_GRAPH_ANALYSER_H_
#define MEDTESTER_GRAPH_ANALYSER_H_

#include "typedefs.h"
#include "OutputMode.h"
#include "Parser.h"

#include <string>
#include <iostream>
#include <exception>


namespace MEDTester
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
    GraphAnalyser(MEDTester::Parser& parser);
    GraphAnalyser(const std::string& inputFilename, const std::string& outputFilename, MEDTester::OutputMode outputMode, bool showTime);
    ~GraphAnalyser();

    // Standard analysis function
    void analyze() const;

private:
    // Options for graph analysis (set in constructor):
    std::string mInputFilename;
    std::string mOutputFilename;
    MEDTester::OutputMode mOutputMode;
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
    static MEDTester::Matrix getAdjList(std::istream& in, int graphNum, bool errorCheck);
};

} // namespace MEDTester

#endif // MEDTESTER_GRAPH_ANALYSER_H_
