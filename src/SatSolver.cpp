#include "SatSolver.h"

#include "typedefs.h"

#include <cryptominisat5/cryptominisat.h>

#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>


#define EDGE_TYPES_COUNT 5
#define VERTEX_TYPES_COUNT 15


void MEDTester::SatSolver::init()
{
    int n = mGraph.getVerticesCount();
    MEDTester::Matrix adjList = mGraph.getAdjList();

    mEdgeVarNums = MEDTester::Matrix(n, MEDTester::MatrixLine(n, -1));
    mEdgeVars = std::vector<std::pair<int,int>>();

    int edgeVarNum = 0;
    for (int u = 0; u < n; ++u) {
        for (int i = 0; i < 3; ++i) {
            int v = adjList[u][i];
            if (mEdgeVarNums[v][u] != -1) {
                mEdgeVarNums[u][v] = mEdgeVarNums[v][u];
            } else {
                mEdgeVarNums[u][v] = edgeVarNum;
                mEdgeVars.push_back({u, v});
                ++edgeVarNum;
            }
        }
    }
}


MEDTester::SatSolver::SatSolver(const MEDTester::CubicGraph& graph) : mGraph(graph)
{
    init();
    solve();
}

MEDTester::SatSolver::SatSolver(const MEDTester::CubicGraph& graph, unsigned int numThreads) : mGraph(graph)
{
    init();
    mSolver.set_num_threads(numThreads);
    solve();
}

MEDTester::SatSolver::~SatSolver() {}


bool MEDTester::SatSolver::isDecomposable() const
{
    return mDecomposable;
}

MEDTester::Decomposition MEDTester::SatSolver::getDecomposition()
{
    if (mDecomposable && mDecomposition.empty()) {
        mDecomposition = MEDTester::Decomposition(mGraph.getVerticesCount(), std::vector<MEDTester::EdgeType>(3, MEDTester::EdgeType::NONE));
        std::vector<CMSat::lbool> model = mSolver.get_model();
        MEDTester::Matrix adjListIndices = mGraph.getAdjListIndices();

        for (int var = 0; var < mGraph.getEdgesCount() * EDGE_TYPES_COUNT; ++var) {
            if (model[var] == CMSat::l_True) {
                std::pair<std::pair<int,int>, MEDTester::SatEdgeType> edgeVar = edgeVarFromNum(var);
                int u = edgeVar.first.first;
                int v = edgeVar.first.second;
                MEDTester::EdgeType type = set2et(edgeVar.second);
                mDecomposition[u][adjListIndices[u][v]] = type;
                mDecomposition[v][adjListIndices[v][u]] = type;
            }
        }
    }

    return mDecomposition;
}

bool MEDTester::SatSolver::generateNextDecomposition()
{
    return false;
}


int MEDTester::SatSolver::edgeVarToNum(int v1, int v2, MEDTester::SatEdgeType type) const
{
    return mEdgeVarNums[v1][v2] * EDGE_TYPES_COUNT + (int) type;
}

std::pair<std::pair<int,int>, MEDTester::SatEdgeType> MEDTester::SatSolver::edgeVarFromNum(int var) const
{
    if (var >= mGraph.getEdgesCount() * EDGE_TYPES_COUNT) {
        return {};
    }

    MEDTester::SatEdgeType type = (MEDTester::SatEdgeType) (var % EDGE_TYPES_COUNT);
    std::pair<int,int> edge = mEdgeVars[var / EDGE_TYPES_COUNT];
    return {edge, type};
}

MEDTester::EdgeType MEDTester::SatSolver::set2et(MEDTester::SatEdgeType type) const
{
    switch (type) {
        case MEDTester::SatEdgeType::MATCHING:
            return MEDTester::EdgeType::MATCHING;
        case MEDTester::SatEdgeType::CYCLE_EVEN:
            return MEDTester::EdgeType::CYCLE;
        case MEDTester::SatEdgeType::CYCLE_ODD:
            return MEDTester::EdgeType::CYCLE;
        case MEDTester::SatEdgeType::STAR_LEAF:
            return MEDTester::EdgeType::STAR_LEAF;
        case MEDTester::SatEdgeType::STAR_CENTER:
            return MEDTester::EdgeType::STAR_CENTER;
    }

    return MEDTester::EdgeType::NONE;
}


void MEDTester::SatSolver::createTheory()
{
    int verticesCount = mGraph.getVerticesCount();
    int edgesCount = mGraph.getEdgesCount();
    MEDTester::Matrix adjList = mGraph.getAdjList();
    MEDTester::Matrix adjListIndices = mGraph.getAdjListIndices();

    /* Var e(uv,T) for each edge uv and each edge type T. If e(uv,T) is set to true, it means
     * edge uv is of type T. */
    mSolver.new_vars(edgesCount * EDGE_TYPES_COUNT);

    /* Each edge should have only one edge type, which can be made using following clauses. */
    for (int e = 0; e < edgesCount; ++e) {
        /* Each edge must have at least one type. */
        MEDTester::Clause clause;
        for (int t = 0; t < EDGE_TYPES_COUNT; ++t) {
            clause.push_back(CMSat::Lit(e * EDGE_TYPES_COUNT + t, false));
        }
        mSolver.add_clause(clause);

        /* Each edge can't have more than one type. */
        for (int t1 = 0; t1 < EDGE_TYPES_COUNT - 1; ++t1) {
            for (int t2 = t1 + 1; t2 < EDGE_TYPES_COUNT; ++t2) {
                mSolver.add_clause({CMSat::Lit(e * EDGE_TYPES_COUNT + t1, true), CMSat::Lit(e * EDGE_TYPES_COUNT + t2, true)});
            }
        }
    }

    for (int v = 0; v < verticesCount; ++v) {
        /* Adjacent edges can't be both matching edges. */
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::MATCHING), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::MATCHING), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::MATCHING), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::MATCHING), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::MATCHING), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::MATCHING), true)
        });

        /* Adjacent edges can't be both cycle edges with same "parity". */
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::CYCLE_EVEN), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::CYCLE_EVEN), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::CYCLE_EVEN), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::CYCLE_EVEN), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::CYCLE_EVEN), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::CYCLE_EVEN), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::CYCLE_ODD), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::CYCLE_ODD), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][0], MEDTester::SatEdgeType::CYCLE_ODD), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::CYCLE_ODD), true)
        });
        mSolver.add_clause({
            CMSat::Lit(edgeVarToNum(v, adjList[v][1], MEDTester::SatEdgeType::CYCLE_ODD), true),
            CMSat::Lit(edgeVarToNum(v, adjList[v][2], MEDTester::SatEdgeType::CYCLE_ODD), true)
        });
    }

    // matrix for effective double-star independence checking
    std::vector<std::vector<bool>> done(edgesCount, std::vector<bool>(edgesCount, false));

    for (int e = 0; e < edgesCount; ++e) {
        int u = mEdgeVars[e].first;
        int v = mEdgeVars[e].second;
        int i = adjListIndices[u][v];
        int j = adjListIndices[v][u];

        /* Each "odd" cycle edge has exactly one adjacent "even" edge on each side and vice versa.
         * In other words, if an edge is cycle even edge, than it has two adjacent cycle odd edges
         * and vice versa (but those edges can't be adjacent with each other because of previoulys
         * added clauses). */
        CMSat::Lit cycleEvenLit(edgeVarToNum(u, v, MEDTester::SatEdgeType::CYCLE_EVEN), true);
        CMSat::Lit cycleOddLit(edgeVarToNum(u, v, MEDTester::SatEdgeType::CYCLE_ODD), true);

        mSolver.add_clause({
            cycleEvenLit,
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+1)%3], MEDTester::SatEdgeType::CYCLE_ODD), false),
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+2)%3], MEDTester::SatEdgeType::CYCLE_ODD), false)
        });
        mSolver.add_clause({
            cycleEvenLit,
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+1)%3], MEDTester::SatEdgeType::CYCLE_ODD), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+2)%3], MEDTester::SatEdgeType::CYCLE_ODD), false)
        });
        
        mSolver.add_clause({
            cycleOddLit,
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+1)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false),
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+2)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false)
        });
        mSolver.add_clause({
            cycleOddLit,
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+1)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+2)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false)
        });

        /* Double-star center edge should be adjacent to only Double-star leaf edges. */
        CMSat::Lit starCenterLit(edgeVarToNum(u, v, MEDTester::SatEdgeType::STAR_CENTER), true);

        mSolver.add_clause({starCenterLit, CMSat::Lit(edgeVarToNum(u, adjList[u][(i+1)%3], MEDTester::SatEdgeType::STAR_LEAF), false)});
        mSolver.add_clause({starCenterLit, CMSat::Lit(edgeVarToNum(u, adjList[u][(i+2)%3], MEDTester::SatEdgeType::STAR_LEAF), false)});
        mSolver.add_clause({starCenterLit, CMSat::Lit(edgeVarToNum(v, adjList[v][(j+1)%3], MEDTester::SatEdgeType::STAR_LEAF), false)});
        mSolver.add_clause({starCenterLit, CMSat::Lit(edgeVarToNum(v, adjList[v][(j+2)%3], MEDTester::SatEdgeType::STAR_LEAF), false)});

        /* Each double-star leaf edge should have one adjacent double-star center edge and
         * one adjacent cycle edge (both even and odd cycle edge, but that is assured in previous
         * clauses). We don't need to add clauses telling that those two edges can't be adjacent,
         * because that is implied by other clauses. */
        CMSat::Lit starLeafLit(edgeVarToNum(u, v, MEDTester::SatEdgeType::STAR_LEAF), true);

        mSolver.add_clause({
            starLeafLit,
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+1)%3], MEDTester::SatEdgeType::STAR_CENTER), false),
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+2)%3], MEDTester::SatEdgeType::STAR_CENTER), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+1)%3], MEDTester::SatEdgeType::STAR_CENTER), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+2)%3], MEDTester::SatEdgeType::STAR_CENTER), false)
        });

        mSolver.add_clause({
            starLeafLit,
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+1)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false),
            CMSat::Lit(edgeVarToNum(u, adjList[u][(i+2)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+1)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false),
            CMSat::Lit(edgeVarToNum(v, adjList[v][(j+2)%3], MEDTester::SatEdgeType::CYCLE_EVEN), false)
        });

        /* Double-stars should be indenpendent - for each edge, he shouldn't have more than one 
         * adjacent double-star leaf edge unless it is double-star center. */
        CMSat::Lit starCenterLit2(edgeVarToNum(u, v, MEDTester::SatEdgeType::STAR_CENTER), false);
        for (int a = 1; a <= 2; ++a) {
            for (int b = 1; b <= 2; ++b) {
                int x = adjList[u][(i+a)%3];
                int y = adjList[v][(j+b)%3];
                if (!done[mEdgeVarNums[u][x]][mEdgeVarNums[v][y]]) {
                    mSolver.add_clause({
                        starCenterLit2,
                        CMSat::Lit(edgeVarToNum(u, x, MEDTester::SatEdgeType::STAR_LEAF), true),
                        CMSat::Lit(edgeVarToNum(v, y, MEDTester::SatEdgeType::STAR_LEAF), true),
                    });
                    done[mEdgeVarNums[u][x]][mEdgeVarNums[v][y]] = true;
                    done[mEdgeVarNums[v][y]][mEdgeVarNums[u][x]] = true;
                }
            }
        }
    }
}


void MEDTester::SatSolver::solve()
{
    // mSolver.log_to_file("sat.log");
    createTheory();
    mDecomposable = mSolver.solve() == CMSat::l_True ? true : false;
}
