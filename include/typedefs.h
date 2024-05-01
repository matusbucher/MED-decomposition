#ifndef MEDTESTER_TYPEDEF_H_
#define MEDTESTER_TYPEDEF_H_

#include <vector>


namespace MEDTester
{

enum class EdgeType
{
    NONE,
    MATCHING,
    CYCLE,
    STAR_POINT,
    STAR_CENTER
};

typedef std::vector<int> MatrixLine;

typedef std::vector<MatrixLine> Matrix;

typedef std::vector<std::vector<EdgeType>> Decomposition;

} // namespace MEDTester

#endif // MEDTESTER_TYPEDEF_H_
