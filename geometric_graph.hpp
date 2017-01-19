#ifndef geometric_graph_hpp
#define geometric_graph_hpp


#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>


struct Point {
  Point() {
    coords.push_back(0);
    coords.push_back(0);
  }
  Point(double x, double y)
  {
    coords.push_back(x);
    coords.push_back(y);
  }
  inline double operator[](int i) const { return coords[i]; }
  inline double operator[](int i) { return coords[i]; }
  friend std::ostream& operator<< (std::ostream& stream, const Point& point);
  double norm() const;
  std::vector<double> coords;
};


Point diff(Point a, Point b);

double dot(Point a, Point b);


double dist(Point a, Point b);

bool operator ==(const Point a, const Point b);
Point operator *(const Point a, double b);
Point operator -(const Point a, const Point b);


typedef int vertex_id;

struct HalfEdge {
  HalfEdge(vertex_id to, double weight) : to(to), weight(weight) { }
  double weight;
  vertex_id to;
};

struct NeighborInfo {
  Point pos;
  vertex_id closest;
};

class GeometricGraph {
public:
  GeometricGraph(double eps)
    : nverts(0), nedges(0), eps(eps) { }


  vertex_id add_vertex(Point);

  int V() const { return nverts; }
  int E() const { return nedges; }
  double Eps() const { return eps; }

  Point position(vertex_id v) const;
  const std::vector<HalfEdge> links(vertex_id v) const;

  vertex_id closest(Point pos) const;
  
  void shrink(double new_eps);

private:
  int nverts, nedges;
  double eps;
  std::vector<Point> coords;
  std::vector<std::vector<HalfEdge> > adj;
};

#endif
