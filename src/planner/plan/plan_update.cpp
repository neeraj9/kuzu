#include "binder/query/updating_clause/bound_create_clause.h"
#include "binder/query/updating_clause/bound_delete_clause.h"
#include "binder/query/updating_clause/bound_merge_clause.h"
#include "binder/query/updating_clause/bound_set_clause.h"
#include "planner/logical_plan/logical_operator/logical_merge.h"
#include "planner/query_planner.h"

using namespace kuzu::common;

namespace kuzu {
namespace planner {

void QueryPlanner::planUpdatingClause(
    binder::BoundUpdatingClause& updatingClause, std::vector<std::unique_ptr<LogicalPlan>>& plans) {
    for (auto& plan : plans) {
        planUpdatingClause(updatingClause, *plan);
    }
}

void QueryPlanner::planUpdatingClause(BoundUpdatingClause& updatingClause, LogicalPlan& plan) {
    switch (updatingClause.getClauseType()) {
    case ClauseType::CREATE: {
        planCreateClause(updatingClause, plan);
        return;
    }
    case ClauseType::MERGE: {
        planMergeClause(updatingClause, plan);
        return;
    }
    case ClauseType::SET: {
        planSetClause(updatingClause, plan);
        return;
    }
    case ClauseType::DELETE_: {
        planDeleteClause(updatingClause, plan);
        return;
    }
    default:
        throw NotImplementedException("QueryPlanner::planUpdatingClause");
    }
}

void QueryPlanner::planCreateClause(
    binder::BoundUpdatingClause& updatingClause, LogicalPlan& plan) {
    auto& createClause = (BoundCreateClause&)updatingClause;
    if (plan.isEmpty()) { // E.g. CREATE (a:Person {age:20})
        appendDummyScan(plan);
    } else {
        appendAccumulate(common::AccumulateType::REGULAR, plan);
    }
    if (createClause.hasNodeInfo()) {
        appendCreateNode(createClause.getNodeInfos(), plan);
    }
    if (createClause.hasRelInfo()) {
        appendCreateRel(createClause.getRelInfos(), plan);
    }
}

void QueryPlanner::planMergeClause(binder::BoundUpdatingClause& updatingClause, LogicalPlan& plan) {
    auto& mergeClause = (BoundMergeClause&)updatingClause;
    auto queryGraphCollection = mergeClause.getQueryGraphCollection();
    binder::expression_vector predicates;
    if (mergeClause.hasPredicate()) {
        predicates = mergeClause.getPredicate()->splitOnAND();
    }
    planOptionalMatch(*mergeClause.getQueryGraphCollection(), predicates, plan);
    // TODO(Xiyang): fix mark
    auto rels = queryGraphCollection->getQueryRels();
    auto nodes = queryGraphCollection->getQueryNodes();
    auto mark =
        !rels.empty() ? rels[0]->getInternalIDProperty() : nodes[0]->getInternalIDProperty();
    std::vector<std::unique_ptr<LogicalCreateNodeInfo>> logicalCreateNodeInfos;
    std::vector<std::unique_ptr<LogicalSetPropertyInfo>> logicalCreateNodeSetInfos;
    if (mergeClause.hasCreateNodeInfo()) {
        auto boundCreateNodeInfos = mergeClause.getCreateNodeInfos();
        for (auto& info : boundCreateNodeInfos) {
            logicalCreateNodeInfos.push_back(createLogicalCreateNodeInfo(info));
        }
        for (auto& info : createLogicalSetPropertyInfo(boundCreateNodeInfos)) {
            logicalCreateNodeSetInfos.push_back(createLogicalSetPropertyInfo(info.get()));
        }
    }
    std::vector<std::unique_ptr<LogicalCreateRelInfo>> logicalCreateRelInfos;
    if (mergeClause.hasCreateRelInfo()) {
        for (auto& info : mergeClause.getCreateRelInfos()) {
            logicalCreateRelInfos.push_back(createLogicalCreateRelInfo(info));
        }
    }
    std::vector<std::unique_ptr<LogicalSetPropertyInfo>> logicalOnCreateSetNodeInfos;
    if (mergeClause.hasOnCreateSetNodeInfo()) {
        for (auto& info : mergeClause.getOnCreateSetNodeInfos()) {
            logicalOnCreateSetNodeInfos.push_back(createLogicalSetPropertyInfo(info));
        }
    }
    std::vector<std::unique_ptr<LogicalSetPropertyInfo>> logicalOnCreateSetRelInfos;
    if (mergeClause.hasOnCreateSetRelInfo()) {
        for (auto& info : mergeClause.getOnCreateSetRelInfos()) {
            logicalOnCreateSetRelInfos.push_back(createLogicalSetPropertyInfo(info));
        }
    }
    std::vector<std::unique_ptr<LogicalSetPropertyInfo>> logicalOnMatchSetNodeInfos;
    if (mergeClause.hasOnMatchSetNodeInfo()) {
        for (auto& info : mergeClause.getOnMatchSetNodeInfos()) {
            logicalOnMatchSetNodeInfos.push_back(createLogicalSetPropertyInfo(info));
        }
    }
    std::vector<std::unique_ptr<LogicalSetPropertyInfo>> logicalOnMatchSetRelInfos;
    if (mergeClause.hasOnMatchSetRelInfo()) {
        for (auto& info : mergeClause.getOnMatchSetRelInfos()) {
            logicalOnMatchSetRelInfos.push_back(createLogicalSetPropertyInfo(info));
        }
    }
    auto merge = std::make_shared<LogicalMerge>(mark, std::move(logicalCreateNodeInfos),
        std::move(logicalCreateNodeSetInfos), std::move(logicalCreateRelInfos),
        std::move(logicalOnCreateSetNodeInfos), std::move(logicalOnCreateSetRelInfos),
        std::move(logicalOnMatchSetNodeInfos), std::move(logicalOnMatchSetRelInfos),
        plan.getLastOperator());
    appendFlattens(merge->getGroupsPosToFlatten(), plan);
    merge->setChild(0, plan.getLastOperator());
    merge->computeFactorizedSchema();
    plan.setLastOperator(merge);
}

void QueryPlanner::planSetClause(binder::BoundUpdatingClause& updatingClause, LogicalPlan& plan) {
    appendAccumulate(common::AccumulateType::REGULAR, plan);
    auto& setClause = (BoundSetClause&)updatingClause;
    if (setClause.hasNodeInfo()) {
        appendSetNodeProperty(setClause.getNodeInfos(), plan);
    }
    if (setClause.hasRelInfo()) {
        appendSetRelProperty(setClause.getRelInfos(), plan);
    }
}

void QueryPlanner::planDeleteClause(
    binder::BoundUpdatingClause& updatingClause, LogicalPlan& plan) {
    appendAccumulate(common::AccumulateType::REGULAR, plan);
    auto& deleteClause = (BoundDeleteClause&)updatingClause;
    if (deleteClause.hasRelInfo()) {
        appendDeleteRel(deleteClause.getRelInfos(), plan);
    }
    if (deleteClause.hasNodeInfo()) {
        appendDeleteNode(deleteClause.getNodeInfos(), plan);
    }
}

} // namespace planner
} // namespace kuzu
