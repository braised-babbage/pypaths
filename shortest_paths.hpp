#ifndef shortest_paths_hpp
#define shortest_paths_hpp

#include <queue>
#include <vector>
#include <utility>
#include "geometric_graph.hpp"

typedef std::pair<double, vertex_id> weighted_vertex;

class ShortestPaths {
public:
  ShortestPaths(const GeometricGraph &g, vertex_id start);
  double dist(vertex_id end) const;
  bool hasPath(vertex_id end) const;
  std::vector<HalfEdge> pathTo(vertex_id end) const;
private:
  void initialize_queue(const GeometricGraph& g, vertex_id start);
  void relax(vertex_id v, HalfEdge l);
  std::priority_queue<weighted_vertex, std::vector<weighted_vertex>, std::greater<weighted_vertex> > pq;
  vertex_id start;
  std::vector<double> d;
  std::vector<HalfEdge> pre;
};

#endif
