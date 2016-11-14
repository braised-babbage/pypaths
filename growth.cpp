#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"

using std::cout;
using std::endl;
using std::abs;
using std::max;

const bool DUMP_INFO = true;


double eps(int n) {
  //return 1.0 / pow(n, 0.35);
  return 2.0;
};

class LineDistance {
public:
  LineDistance(Point2D pa, Point2D pb) 
  {
    double d = dist(pa,pb);
    double vx = pb.x - pa.x;
    double vy = pb.y - pa.y;
    nx = -vy/d;
    ny = vx/d;
    c = nx*pa.x + ny*pa.y;
  }

  double operator()(Point2D p) {
    return abs(nx*p.x + ny*p.y - c); 
  }
  
private:
  double nx, ny;
  double c;
};

class PathStatistics {
public:
  PathStatistics(Point2D pa, Point2D pb, const GeometricGraph& g) {
    vertex_id a = g.closest(pa);
    vertex_id b = g.closest(pb);
    
    Point2D nna = g.position(a);
    Point2D nnb = g.position(b);

    

    ShortestPaths sp(g,a);
    nn_dist = dist(pa,nna) + dist(pb,nnb);
    length = sp.dist(b) + nn_dist;

    LineDistance ld(pa,pb);
    wander = max(ld(nna), ld(nnb));
    auto path = sp.pathTo(b);
    for (auto l = path.begin(); l != path.end(); l++) {
      wander = max(wander, ld(g.position(l->to)));
    }
  };

  double nn_distance() {
    return nn_dist;
  }

  double path_length() {
    return length;
  }

  double wander_distance() {
    return wander;
  }
private:

double length;
double wander;
double nn_dist;
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
  g.add_vertex(pa);
  g.add_vertex(pb);
  g.add_vertex(pc);

  Point2D u(-1,0);
  Point2D v(2,1);

  PathStatistics ps(u,v,g);
  assert(ps.path_length() == 4);
  double t = 0.0001;
  assert(abs(ps.wander_distance() - 0.632456) < t);
};



void RGG(int n, int interval, int initial) {
  std::uniform_real_distribution<double> unif(-0.5,0.5);
  std::default_random_engine re;
  std::random_device rd;
  re.seed(rd());

  Point2D pa(-0.25,-0.25);
  Point2D pb(0.25,0.25);
  
  GeometricGraph g(eps(initial));
 
  for (int i = 0; i < n; i++) {
    double x = unif(re);
    double y = unif(re);
    g.add_vertex(Point2D(x,y));
    
    if (i > initial && i % interval == 0) {
      g.shrink(eps(i));
      PathStatistics ps(pa, pb, g);
	if (ps.path_length() != M) {
	  if (DUMP_INFO) {
	    std::cout << "i = " << i 
		      << " distance = " << ps.path_length()
		      << " nn dist = " << ps.nn_distance()
		      << " wander = " << ps.wander_distance() << std::endl;
	}
      }
    }
  }
}

int main() {
  TestLineDistance();
  TestPathStatistics();
  RGG(10000,10,50);
  return 0;
}
