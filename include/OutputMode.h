#ifndef MEDTESTER_OUTPUT_MODE_H_
#define MEDTESTER_OUTPUT_MODE_H_


namespace MEDTester
{

enum class OutputMode
{
    ONLY_RESULT,
    NOT_DECOMPOSABLE,
    NOT_DECOMPOSABLE_BRIDGELESS,
    COLORING,
    COUNT
};
    
} // namespace MEDTester

#endif // MEDTESTER_OUTPUT_MODE_H_
