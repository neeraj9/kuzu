#pragma once

#include <cassert>

#include "src/expression_evaluator/include/expression_evaluator.h"
#include "src/processor/include/physical_plan/expression_mapper.h"
#include "src/processor/include/physical_plan/operator/physical_operator.h"

using namespace graphflow::evaluator;

namespace graphflow {
namespace processor {

class Projection : public PhysicalOperator {

public:
    Projection(unique_ptr<vector<unique_ptr<ExpressionEvaluator>>> expressions,
        vector<uint64_t> expressionPosToDataChunkPos, const vector<uint64_t>& discardedDataChunkPos,
        unique_ptr<PhysicalOperator> prevOperator);

    void getNextTuples() override;

    unique_ptr<PhysicalOperator> clone() override;

private:
    unique_ptr<vector<unique_ptr<ExpressionEvaluator>>> expressions;
    vector<uint64_t> expressionPosToDataChunkPos;
    vector<uint64_t> discardedDataChunkPos;
    shared_ptr<ResultSet> discardedResultSet;
    shared_ptr<ResultSet> inResultSet;
};

} // namespace processor
} // namespace graphflow
