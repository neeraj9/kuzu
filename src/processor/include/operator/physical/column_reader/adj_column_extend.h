#pragma once

#include "src/processor/include/operator/physical/column_reader/column_reader.h"

namespace graphflow {
namespace processor {

class AdjColumnExtend : public ColumnReader {

public:
    AdjColumnExtend(const uint64_t& inDataChunkIdx, const uint64_t& inValueVectorIdx,
        BaseColumn* column, unique_ptr<Operator> prevOperator);

    unique_ptr<Operator> clone() override {
        return make_unique<AdjColumnExtend>(
            inDataChunkIdx, inValueVectorIdx, column, move(prevOperator->clone()));
    }
};

} // namespace processor
} // namespace graphflow
