#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;

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


int main() {
  GeometricGraphTests();
  ClosestPointTests();
  return 0;
}
