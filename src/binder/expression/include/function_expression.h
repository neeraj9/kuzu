#pragma once

#include "expression.h"

#include "src/function/include/vector_operations.h"

using namespace graphflow::function;

namespace graphflow {
namespace binder {

class FunctionExpression : public Expression {

public:
    FunctionExpression(ExpressionType expressionType, DataType dataType, const string& uniqueName,
        bool isDistinct = false)
        : Expression{expressionType, dataType, uniqueName}, isDistinct{isDistinct} {}

    FunctionExpression(ExpressionType expressionType, DataType dataType,
        const shared_ptr<Expression>& child, bool isDistinct = false)
        : Expression{expressionType, dataType, child}, isDistinct{isDistinct} {}

    FunctionExpression(ExpressionType expressionType, DataType dataType, expression_vector children,
        bool isDistinct)
        : Expression{expressionType, dataType, move(children)}, isDistinct{isDistinct} {}

    bool isFunctionDistinct() const {
        assert(isExpressionAggregate(expressionType));
        return isDistinct;
    }

private:
    bool isDistinct;
};

class ScalarFunctionExpression : public FunctionExpression {

public:
    ScalarFunctionExpression(ExpressionType expressionType, DataType dataType,
        expression_vector children, scalar_exec_func execFunc, scalar_select_func selectFunc)
        : FunctionExpression{expressionType, dataType, move(children), false /* isDistinct*/},
          execFunc{move(execFunc)}, selectFunc{move(selectFunc)} {
        assert(dataType == BOOL);
    }

    ScalarFunctionExpression(ExpressionType expressionType, DataType dataType,
        expression_vector children, scalar_exec_func execFunc)
        : FunctionExpression{expressionType, dataType, move(children), false /* isDistinct*/},
          execFunc{move(execFunc)} {
        assert(dataType != BOOL);
    }

public:
    scalar_exec_func execFunc;
    scalar_select_func selectFunc;
};

} // namespace binder
} // namespace graphflow