#ifndef MEDTESTER_PARSER_H_
#define MEDTESTER_PARSER_H_

#include "OutputMode.h"

#include <string>
#include <vector>
#include <exception>

namespace MEDTester
{

class Parser
{
public:
    // Exception thrown when the syntax of the command (program arguments) is in wrong format
    class InvalidSyntaxException : public std::exception
    {
        private:
            std::string message = "Invalid Syntax: ";

        public:
            InvalidSyntaxException(const std::string& msg) throw() { message += msg; }
            virtual ~InvalidSyntaxException() throw() {}
            const char* what() const throw() { return message.c_str(); }
    };

    // Information about specific option
    template <typename T> struct OptionInfo
    {
        const std::string name;
        const std::string description;
        const std::vector<std::string> specifiers;
        const bool hasArg;
        const T defaultValue;
    };

    // Information about specific (output) mode
    template <typename T> struct ModeInfo
    {
        const std::string name;
        const std::string description;
        const std::vector<std::string> specifiers;
        const T value;
    };

    // Option infos:
    static const MEDTester::Parser::OptionInfo<std::string> INPUT_FILENAME_OPTION_INFO;
    static const MEDTester::Parser::OptionInfo<std::string> OUTPUT_FILENAME_OPTION_INFO;
    static const MEDTester::Parser::OptionInfo<MEDTester::OutputMode> OUTPUT_MODE_OPTION_INFO;
    static const MEDTester::Parser::OptionInfo<bool> SHOW_TIME_OPTION_INFO;

    // Mode infos:
    static const MEDTester::Parser::ModeInfo<MEDTester::OutputMode> ONLY_RESULT_MODE_INFO;
    static const MEDTester::Parser::ModeInfo<MEDTester::OutputMode> NOT_DECOMPOSABLE_MODE_INFO;
    static const MEDTester::Parser::ModeInfo<MEDTester::OutputMode> NOT_DECOMPOSABLE_BRIDGELESS_MODE_INFO;
    static const MEDTester::Parser::ModeInfo<MEDTester::OutputMode> COLORING_MODE_INFO;

    static const std::vector<MEDTester::Parser::ModeInfo<MEDTester::OutputMode>> AVAILABLE_OUTPUTMODES;

    // Help speciefier:
    static const std::string HELP_SPECIFIER;

    // Constructor and deconstructor:
    Parser(char** begin, char** end);
    ~Parser();

    // Getters and setters:
    char** getBegin() const;
    char** getEnd() const;
    std::string getInputFilename() const;
    std::string getOutputFilename() const;
    MEDTester::OutputMode getOutputMode() const;
    bool getShowTime() const;

    void setArgumentRange(char** begin, char** end);
    void setInputFilename(const std::string& filename);
    void setOutputFilename(const std::string& filename);
    void setOutputMode(MEDTester::OutputMode outputMode);
    void setShowTime(bool showTime);

    // Static and nonstatic parsing tools:
    static void checkSyntax(char** begin, char** end);
    static bool optionExists(char** begin, char** end, const std::string& option);
    static std::string getOptionArgument(char** begin, char** end, const std::string& option);

    void checkSyntax() const;
    bool optionExists(const std::string& option) const;
    std::string getOptionArgument(const std::string& option) const;

    // Command options parsing:
    bool parseInputFilename();
    bool parseOutputFilename();
    bool parseOutputMode();
    bool parseShowTime();

    void parseAll();

    // Help message:
    static void printHelpMessage();

private:
    // Argument values range:
    char** mBegin;       // Pointer to the begin of argument values array.
    char** mEnd;         // Pointer to the end of argument values array.

    // Parsed values:
    std::string mInputFilename;          // If empty, stdin should be used.
    std::string mOutputFilename;         // If empty, stdout should be used.
    MEDTester::OutputMode mOutputMode;
    bool mShowTime;

    // Private static constants, messages and utility functions:
    static const std::string INVALID_ARGUMENT_RANGE_MESSAGE;
    static inline std::string UNKNOWN_OPTION_MESSAGE(const std::string& option);
    static inline std::string DOUBLE_OPTION_MESSAGE(const std::string& option);
    static inline std::string EXPECTED_OPTION_ARGUMENT_MESSAGE(const std::string& option);
    static inline std::string UNKNOWN_OUTPUT_MODE_MESSAGE(const std::string& mode);

    static std::string formatText(const std::string& text, size_t indent, size_t width);
    static std::string joinToString(std::vector<std::string> list, const std::string& prefix, const std::string& suffix, const std::string& separator);
};

} // namespace MEDTester

#endif // MEDTESTER_PARSER_H_
