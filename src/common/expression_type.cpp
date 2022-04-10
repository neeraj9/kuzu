#include "src/common/include/expression_type.h"

#include <stdexcept>

namespace graphflow {
namespace common {

bool isExpressionUnary(ExpressionType type) {
    return NOT == type || NEGATE == type || IS_NULL == type || IS_NOT_NULL == type;
}

bool isExpressionBinary(ExpressionType type) {
    return isExpressionComparison(type) || isExpressionStringOperator(type) || OR == type ||
           XOR == type || AND == type || ADD == type || SUBTRACT == type || MULTIPLY == type ||
           DIVIDE == type || MODULO == type || POWER == type;
}

bool isExpressionBoolConnection(ExpressionType type) {
    return OR == type || XOR == type || AND == type || NOT == type;
}

bool isExpressionComparison(ExpressionType type) {
    return EQUALS == type || NOT_EQUALS == type || GREATER_THAN == type ||
           GREATER_THAN_EQUALS == type || LESS_THAN == type || LESS_THAN_EQUALS == type;
}

bool isExpressionArithmetic(ExpressionType type) {
    return ADD == type || SUBTRACT == type || MULTIPLY == type || DIVIDE == type ||
           MODULO == type || POWER == type || NEGATE == type;
}

bool isExpressionStringOperator(ExpressionType type) {
    return STARTS_WITH == type || ENDS_WITH == type || CONTAINS == type;
}

bool isExpressionNullOperator(ExpressionType type) {
    return IS_NULL == type || IS_NOT_NULL == type;
}

bool isExpressionLiteral(ExpressionType type) {
    return LITERAL_INT == type || LITERAL_DOUBLE == type || LITERAL_STRING == type ||
           LITERAL_BOOLEAN == type || LITERAL_DATE == type || LITERAL_TIMESTAMP == type ||
           LITERAL_INTERVAL == type || LITERAL_NULL == type;
}

bool isExpressionAggregate(ExpressionType type) {
    return COUNT_STAR_FUNC == type || COUNT_FUNC == type || SUM_FUNC == type || AVG_FUNC == type ||
           MIN_FUNC == type || MAX_FUNC == type;
}

bool isExpressionSubquery(ExpressionType type) {
    return EXISTENTIAL_SUBQUERY == type;
}

string expressionTypeToString(ExpressionType type) {
    switch (type) {
    case OR:
        return "OR";
    case XOR:
        return "XOR";
    case AND:
        return "AND";
    case NOT:
        return "NOT";
    case EQUALS:
        return "EQUALS";
    case NOT_EQUALS:
        return "NOT_EQUALS";
    case GREATER_THAN:
        return "GREATER_THAN";
    case GREATER_THAN_EQUALS:
        return "GREATER_THAN_EQUALS";
    case LESS_THAN:
        return "LESS_THAN";
    case LESS_THAN_EQUALS:
        return "LESS_THAN_EQUALS";
    case ADD:
        return ADD_FUNC_NAME;
    case SUBTRACT:
        return SUBTRACT_FUNC_NAME;
    case MULTIPLY:
        return MULTIPLY_FUNC_NAME;
    case DIVIDE:
        return DIVIDE_FUNC_NAME;
    case MODULO:
        return MODULO_FUNC_NAME;
    case POWER:
        return POWER_FUNC_NAME;
    case NEGATE:
        return NEGATE_FUNC_NAME;
    case STARTS_WITH:
        return "STARTS_WITH";
    case ENDS_WITH:
        return "ENDS_WITH";
    case CONTAINS:
        return "CONTAINS";
    case IS_NULL:
        return "IS_NULL";
    case IS_NOT_NULL:
        return "IS_NOT_NULL";
    case PROPERTY:
        return "PROPERTY";
    case FUNCTION:
        return "FUNCTION";
    case LITERAL_INT:
        return "LITERAL_INT";
    case LITERAL_DOUBLE:
        return "LITERAL_DOUBLE";
    case LITERAL_STRING:
        return "LITERAL_STRING";
    case LITERAL_BOOLEAN:
        return "LITERAL_BOOLEAN";
    case LITERAL_DATE:
        return "LITERAL_DATE";
    case LITERAL_TIMESTAMP:
        return "LITERAL_TIMESTAMP";
    case LITERAL_NULL:
        return "LITERAL_NULL";
    case VARIABLE:
        return "VARIABLE";
    case COUNT_STAR_FUNC:
        return COUNT_STAR_FUNC_NAME;
    case COUNT_FUNC:
        return COUNT_FUNC_NAME;
    case SUM_FUNC:
        return SUM_FUNC_NAME;
    case AVG_FUNC:
        return AVG_FUNC_NAME;
    case MIN_FUNC:
        return MIN_FUNC_NAME;
    case MAX_FUNC:
        return MAX_FUNC_NAME;
    case LIST_EXTRACT:
        return LIST_EXTRACT_FUNC_NAME;
    default:
        throw invalid_argument("Should never happen. Cannot convert expression type to string");
    }
}

} // namespace common
} // namespace graphflow
