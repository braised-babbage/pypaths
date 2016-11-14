#include <vector>
#include <limits>
#include <utility>
#include <queue>

#include "geometric_graph.hpp"
#include "shortest_paths.hpp"

using std::vector;


void ShortestPaths::initialize_queue(const GeometricGraph &g, vertex_id start) {
  pq.push(weighted_vertex(0.0,start));
  for (vertex_id v = 0; v != g.V(); v++) {
    if (v != start)
      pq.push(weighted_vertex(M,v));
  }
}

void ShortestPaths::relax(vertex_id v, HalfEdge l) {
      vertex_id u = l.to;
      double alt = d[v] + l.weight;
      if (alt < d[u]) {
	d[u] = alt;
	pre[u] = HalfEdge(v,l.weight);
	pq.push(weighted_vertex(alt,u));
      }
}

// initializer lists??
ShortestPaths::ShortestPaths(const GeometricGraph &g, vertex_id start) : start(start)  {
  d = vector<double>(g.V(), M);
  pre = vector<HalfEdge>(g.V(), HalfEdge(start,M));

  vector<bool> visited(g.V(), false);
  d[start] = 0.0;

  initialize_queue(g, start);

  while (!pq.empty()) {
    weighted_vertex wv = pq.top();
    pq.pop();
    vertex_id v = std::get<1>(wv);
    
    if (visited[v])
      continue;
    visited[v] = true;
    
    auto links = g.links(v);
    for (auto l = links.begin(); l != links.end(); l++) {
      relax(v, *l);
    }
  }
  
};


double ShortestPaths::dist(vertex_id end) const {
  return d[end];
}

bool ShortestPaths::hasPath(vertex_id end) const {
  return d[end] != std::numeric_limits<double>::max();
};

vector<HalfEdge> ShortestPaths::pathTo(vertex_id end) const {
  vector<HalfEdge> path;
  while (end != start) {
    HalfEdge e = pre[end];
    path.push_back(HalfEdge(end,e.weight));
    end = e.to;
  }
  std::reverse(path.begin(), path.end());
  return path;
};
