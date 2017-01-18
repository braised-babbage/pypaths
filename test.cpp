#include <cassert>
#include <iostream>
#include <random>
#include <cmath>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using std::abs;
using std::cout;
using std::vector;

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

  auto path = sp.pathTo(w);
  assert(path.size() == 2);
  assert(path[0].to == v);
  assert(path[1].to == w);

  g.shrink(0.5);
  ShortestPaths sp2(g,u);
  assert(sp2.hasPath(u));
  assert(!sp2.hasPath(v));

};

void TestLineDistance() {
  Point2D pa(0,0);
  Point2D pb(1,0);
  Point2D pc(0.5,0.5);
  Point2D pd(0.5,-0.5);

  double t = 0.00001;
  LineDistance ld(pa,pb);

  assert(abs(ld(pc) - 0.5) < t);
  assert(abs(ld(pa) - 0) < t);
  assert(abs(ld(pb) - 0) < t);
  assert(abs(ld(pd) - 0.5) < t);

  ld = LineDistance(pa,pc);
  assert(abs(ld(pb) - 0.707107) < t);
  assert(abs(ld(pd) - 0.707107) < t);
}

void TestPathStatistics() {
  Point2D pa(0,0);
  Point2D pb(1,0);
  Point2D pc(1,1);
  GeometricGraph g(1.1);
  vertex_id a = g.add_vertex(pa);
  vertex_id b = g.add_vertex(pb);
  vertex_id c = g.add_vertex(pc);

  Point2D u(-1,0);
  Point2D v(2,1);

  assert(g.closest(u) == a);
  PathStatistics ps(u,v,g);
  assert(ps.path_length() == 4.0);
  assert(ps.num_hops() == 4);
  double t = 0.0001;
  assert(abs(ps.wander_distance() - 0.632456) < t);
}

void TestBallStatistics() {
  Point2D pa(0,0);
  Point2D pb(1,0);
  Point2D pc(1,1);
  GeometricGraph g(1.1);
  vertex_id a = g.add_vertex(pa);
  vertex_id b = g.add_vertex(pb);
  vertex_id c = g.add_vertex(pc);

  Point2D u(-1,0);
  Point2D v(2,1);

  BallStatistics bs(g,u,1.1);
  vector<Point2D> ball = bs.ball();
  assert(std::find(ball.begin(), ball.end(), pa) != ball.end());
  assert(std::find(ball.begin(), ball.end(), pb) == ball.end());
  assert(std::find(ball.begin(), ball.end(), pc) == ball.end());

  BallStatistics bs2(g,u,2.1);
  ball = bs2.ball();
  assert(std::find(ball.begin(), ball.end(), pa) != ball.end());
  assert(std::find(ball.begin(), ball.end(), pb) != ball.end());
  assert(std::find(ball.begin(), ball.end(), pc) == ball.end());
};

int main() {
  GeometricGraphTests();
  ClosestPointTests();
  ShortestPathsTests();
  TestLineDistance();
  TestPathStatistics();
  TestBallStatistics();
  std::cout << "Tests passed!" << std::endl;
  //RGG();
  return 0;
}
