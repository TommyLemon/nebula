// Copyright (c) 2020 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#ifndef GRAPH_EXECUTOR_ALGO_SUBGRAPHEXECUTOR_H_
#define GRAPH_EXECUTOR_ALGO_SUBGRAPHEXECUTOR_H_

#include <robin_hood.h>

#include "graph/executor/Executor.h"

// Subgraph receive result from GetNeighbors
// There are two Main functions
// First : Extract the deduplicated destination VID from GetNeighbors
// Second: Delete previously visited edges and save the result(iter) to the variable `resultVar`
//
// Member:
// `historyVids_` : is hash table
//    KEY   : the VID of the visited destination Vertex
//    VALUE : the number of steps to visit the KEY (starting vertex is 0)
// since each vertex will only be visited once, if it is a one-way edge expansion, there will be no
// duplicate edges. we only need to focus on the case of two-way expansion
//
// How to delete edges:
//  determine whether a loop is formed by the number of steps. If the destination vid has been
//  visited, and the number of steps of the destination vid differs by 2 from the current steps, it
//  is judged that a loop is formed, the edge needs to be deleted
//
// For example: Topology is below
// a->c, a->b, b->a, b->c
// statement: get subgraph from 'a' both edge yield vertices as nodes, edges as relationships
// first steps :  a->b, a->c, a<-b, all edges need to save
// second steps:  b->a, b<-a, b->c, c<-a
// since it is a two-way expansion, the negative edge has already been visited,
// so b<-a & c<-a are deleted
// b->a : the number of steps of the destination vid `a` is 0, and the current steps is 2. it can be
// determined that a loop is formed, so this edge also needs to be deleted.
// b->c : determined by the number of steps that no loop is formed, so keep it

namespace nebula {
namespace graph {
class SubgraphExecutor : public Executor {
 public:
  SubgraphExecutor(const PlanNode* node, QueryContext* qctx)
      : Executor("SubgraphExecutor", node, qctx) {}

  folly::Future<Status> execute() override;

 private:
  robin_hood::unordered_flat_map<Value, int64_t, std::hash<Value>> historyVids_;
};

}  // namespace graph
}  // namespace nebula
#endif  // GRAPH_EXECUTOR_ALGO_SUBGRAPHEXECUTOR_H_
