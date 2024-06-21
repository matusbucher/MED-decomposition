#ifndef MEDTESTER_TYPEDEF_H_
#define MEDTESTER_TYPEDEF_H_

#include <vector>


namespace MEDTester
{

enum class EdgeType
{
    NONE = 0,
    MATCHING = 1,
    CYCLE = 2,
    STAR_LEAF = 3,
    STAR_CENTER = 4
};

typedef std::vector<int> MatrixLine;

typedef std::vector<MatrixLine> Matrix;

typedef std::vector<std::vector<EdgeType>> Decomposition;

} // namespace MEDTester

#endif // MEDTESTER_TYPEDEF_H_
