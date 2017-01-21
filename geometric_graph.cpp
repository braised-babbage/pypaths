#include <cassert>
#include <cmath>
#include <limits>
#include <utility>
#include <queue>
#include <iostream>

#include "geometric_graph.hpp"
#include "shortest_paths.hpp"



using std::vector;

bool operator ==(const Point a, const Point b) {
  return a.coords == b.coords;
}

Point operator *(const Point a, double b) {
  Point p(0);
  for (int i = 0; i < a.dims(); i++) {
    p.coords.push_back(a[i]*b);
  }
  return p;
}

Point operator -(const Point a, const Point b) {
  Point p(0);
  for (int i = 0; i < a.dims(); i++) {
    p.coords.push_back(a[i] - b[i]);
  }
  return p;
}

double dot(Point a, Point b) {
  double t = 0.0;
  for (int i = 0; i < a.dims(); i++) {
    t += a[i]*b[i];
  }
  return t;
}

double Point::norm() const
{ return sqrt(dot(*this,*this)); }

double dist(Point a, Point b) {
  return (a-b).norm();
}

std::ostream& operator<< (std::ostream& stream, const Point& p) {
  stream << p[0];
  for (int i = 1; i < p.dims(); i++)
    stream << "," << p[i];
  return stream;
}




vertex_id GeometricGraph::add_vertex(Point pos) {
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

Point GeometricGraph::position(vertex_id v) const {
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

vertex_id GeometricGraph::closest(Point p) const {
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




