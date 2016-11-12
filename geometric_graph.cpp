#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>
#include <utility>
#include <queue>

#include <iostream>

using std::vector;
using std::pair;
using std::priority_queue;

const double M = std::numeric_limits<double>::max();

struct Point2D {
  Point2D(double x, double y) : x(x), y(y) { }
  double x, y;
};

bool operator ==(const Point2D a, const Point2D b) {
  return a.x == b.x && a.y == b.y;
}

typedef int vertex_id;

struct HalfEdge {
  HalfEdge(vertex_id to, double weight) : to(to), weight(weight) { }
  double weight;
  vertex_id to;
};

struct NeighborInfo {
  Point2D pos;
  vertex_id closest;
};

double dist(Point2D a, Point2D b) {
  double delta_x = a.x - b.x;
  double delta_y = a.y - b.y;
  return sqrt(delta_x*delta_x + delta_y*delta_y);
}

class GeometricGraph {
public:
  GeometricGraph(double eps)
    : nverts(0), nedges(0), eps(eps) { }


  vertex_id add_vertex(Point2D);

  int V() const { return nverts; }
  int E() const { return nedges; }
  double Eps() const { return eps; }

  Point2D position(vertex_id v) const;
  const vector<HalfEdge> links(vertex_id v) const;

  vertex_id closest(Point2D pos) const;
  
  void shrink(double new_eps);

private:
  int nverts, nedges;
  double eps;
  vector<Point2D> coords;
  vector<vector<HalfEdge> > adj;
};

typedef pair<double, vertex_id> weighted_vertex;

class ShortestPaths {
public:
  ShortestPaths(const GeometricGraph &g, vertex_id start);
  double dist(vertex_id end) const;
  bool hasPath(vertex_id end) const;
  vector<HalfEdge> pathTo(vertex_id end) const;
private:
  void initialize_queue(const GeometricGraph& g, vertex_id start);
  void relax(vertex_id v, HalfEdge l);
  priority_queue<weighted_vertex, vector<weighted_vertex>, std::greater<weighted_vertex> > pq;
  vertex_id start;
  vector<double> d;
  vector<HalfEdge> pre;
};

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

vertex_id GeometricGraph::add_vertex(Point2D pos) {
  // nverts == coords.size() == adj.size()
  vertex_id id = nverts;

  coords.push_back(pos);
  
  vector<HalfEdge> out_edges;
  adj.push_back(out_edges);

  for (vertex_id v = 0; v < nverts; v++) {
    double d = dist(pos,coords[v]);
    if (d < eps) {
      HalfEdge out(v,d);
      adj[id].push_back(out);

      HalfEdge in(id,d);
      adj[v].push_back(in);

      nedges++;
    }
  }

  nverts++;

  return id;
};

const vector<HalfEdge> GeometricGraph::links(vertex_id v) const {
  return adj[v];
};

Point2D GeometricGraph::position(vertex_id v) const {
  return coords[v];
};

struct Threshold
{
  Threshold(double t) : t(t) { }
  double t;
  bool operator()(HalfEdge e) const { return e.weight >= t; }
};

void GeometricGraph::shrink(double new_eps) {
  // new_eps < eps
  Threshold t(new_eps);
  int num_edges = 0;
  
  for(auto it = adj.begin(); it != adj.end(); it++) {
    auto del = std::remove_if(it->begin(), it->end(), t);
    it->erase(del, it->end());
    num_edges += it->size();
  }
  nedges = num_edges/2;
};

vertex_id GeometricGraph::closest(Point2D p) const {
  // todo: track points
  if(nverts == 0)
    return -1;

  vertex_id c = 0;
  double min_d = dist(p,coords[c]);
  for(vertex_id i = 1; i < nverts; i++) {
    double d = dist(p,coords[i]);
    if (d < min_d) {
      c = i;
      min_d = d;
    }
  }

  return c;
};




void GeometricGraphTests() {
  // empty graph
  GeometricGraph g(0.5);
  assert(g.Eps() == 0.5);
  assert(g.V() == 0);
  assert(g.E() == 0);

  
  Point2D p(0,0);
  vertex_id v = g.add_vertex(p);
  assert(v == 0);
  assert(g.V() == 1);
  assert(g.E() == 0);
  assert(g.position(v) == p);

  // add a neighboring point
  vertex_id u = g.add_vertex(Point2D(0.1,0.1));
  assert(u == 1);
  assert(g.V() == 2);
  assert(g.E() == 1);

  const vector<HalfEdge> out_edges = g.links(0);
  assert(out_edges.size() == 1);
  HalfEdge out = out_edges[0];
  assert(out.to == u);
  assert(out.weight == 0.1*sqrt(2.0));

  // add an isolated point
  vertex_id w = g.add_vertex(Point2D(1.0,1.0));
  assert(w == 2);
  assert(g.V() == 3);
  assert(g.E() == 1);
  assert(g.links(0).size() == 1);
  assert(g.links(1).size() == 1);
  assert(g.links(2).size() == 0);

  // shrink the graph
  vertex_id w2 = g.add_vertex(Point2D(-0.49,0.0));
  assert(w2 == 3);
  assert(g.links(0).size() == 2);
  assert(g.links(3).size() == 1);
  assert(g.E() == 2);

  g.shrink(0.4);
  assert(g.links(0).size() == 1);
  assert(g.links(3).size() == 0);
  assert(g.E() == 1);

}

void ClosestPointTests() {
  GeometricGraph g(1.0);
  vertex_id u,v,w;

  u = g.add_vertex(Point2D(0,0));
  v = g.add_vertex(Point2D(1,0));
  w = g.add_vertex(Point2D(1,1));

  assert(g.closest(Point2D(0,0)) == u);
  assert(g.closest(Point2D(0.1,0)) == u);
  assert(g.closest(Point2D(1.1,0.9)) == w);

}


void ShortestPathsTests() {
  GeometricGraph g(1.01);
  vertex_id u = g.add_vertex(Point2D(0,0));
  vertex_id v = g.add_vertex(Point2D(1,0));
  vertex_id w = g.add_vertex(Point2D(1,1));

  assert(g.E() == 2);

  ShortestPaths sp(g,u);

  assert(sp.hasPath(u));
  assert(sp.dist(u) == 0.0);
  assert(sp.dist(v) == 1.0);
  assert(sp.dist(w) == 2.0);
};


int main() {
  GeometricGraphTests();
  ClosestPointTests();
  ShortestPathsTests();
  return 0;
}
