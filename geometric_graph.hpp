#ifndef geometric_graph_hpp
#define geometric_graph_hpp


#include <vector>
#include <algorithm>
#include <cmath>


struct Point2D {
  Point2D(double x, double y) : x(x), y(y) { }
  double x, y;
};

double dist(Point2D a, Point2D b);

bool operator ==(const Point2D a, const Point2D b);

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

class GeometricGraph {
public:
  GeometricGraph(double eps)
    : nverts(0), nedges(0), eps(eps) { }


  vertex_id add_vertex(Point2D);

  int V() const { return nverts; }
  int E() const { return nedges; }
  double Eps() const { return eps; }

  Point2D position(vertex_id v) const;
  const std::vector<HalfEdge> links(vertex_id v) const;

  vertex_id closest(Point2D pos) const;
  
  void shrink(double new_eps);

private:
  int nverts, nedges;
  double eps;
  std::vector<Point2D> coords;
  std::vector<std::vector<HalfEdge> > adj;
};

#endif
