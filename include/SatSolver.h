#ifndef MEDTESTER_SAT_SOLVER_H_
#define MEDTESTER_SAT_SOLVER_H_

#include "typedefs.h"
#include "CubicGraph.h"

#include <cryptominisat5/cryptominisat.h>

#include <vector>
#include <utility>


namespace MEDTester
{

typedef std::vector<CMSat::Lit> Clause;

enum class SatEdgeType
{
    MATCHING = 0,
    CYCLE_EVEN = 1,
    CYCLE_ODD = 2,
    STAR_LEAF = 3,
    STAR_CENTER = 4
};

class SatSolver
{
public:
    SatSolver(const MEDTester::CubicGraph& graph);
    SatSolver(const MEDTester::CubicGraph& graph, unsigned int numThreads);
    ~SatSolver();

    bool isDecomposable() const;
    MEDTester::Decomposition getDecomposition();
    bool generateNextDecomposition();

private:
    MEDTester::CubicGraph mGraph;

    MEDTester::Decomposition mDecomposition;
    bool mDecomposable;

    MEDTester::Matrix mEdgeVarNums;
    std::vector<std::pair<int,int>> mEdgeVars;

    CMSat::SATSolver mSolver;

    void init();

    int edgeVarToNum(int v1, int v2, MEDTester::SatEdgeType type) const;
    std::pair<std::pair<int,int>, MEDTester::SatEdgeType> edgeVarFromNum(int var) const;
    MEDTester::EdgeType set2et(MEDTester::SatEdgeType type) const;

    void createTheory();
    void solve();
};

} // namespace MEDTester

#endif // MEDTESTER_SAT_SOLVER_H_
