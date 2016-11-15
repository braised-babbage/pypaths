#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <vector>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"

using std::cout;
using std::endl;
using std::abs;
using std::max;
using std::vector;
using std::ostream;

const bool DUMP_INFO = false;


double eps(int n, double exponent = 0.4) {
  return 1.0 / pow(n, exponent);
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


class SimTable {
public:
  SimTable(int max_n)
    : count(max_n+1,0), pathstats(max_n+1, vector<PathStatistics>())
  { }

  void add_item(int n, PathStatistics& ps) {
    // use exceptions
    assert(ps.path_length() != M);
    assert(ps.nn_distance() != M);
    assert(ps.wander_distance() != M);
    count[n] += 1;
    pathstats[n].push_back(ps);
  }

  vector<int> count;
  vector<vector<PathStatistics> > pathstats;
};

void RGG(int n, int interval, int initial, SimTable& st) {
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
	  st.add_item(i, ps);
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

void dump_table_at(ostream& out, SimTable& st, int n) {
  out << "n = " << n << " eps_exponent = " << 0.4 << " iters = " << st.count[n] << endl;
  out << "path_length nn_distance wander_distance" << endl;
  for(auto it = st.pathstats[n].begin(); it != st.pathstats[n].end(); it++) {
    out << it->path_length() << " " << it->nn_distance() << " " << it->wander_distance() << endl;
  }
  out << endl;
}

void dump_all(ostream& out, SimTable& st) {
  //  out << "eps = " << 0.4 << endl;
  out << "n,path_length,nn_distance,wander_distance" << endl;
  for (int n = 0; n < st.count.size(); n++) {
    if (st.count[n] == 0)
      continue;

    for (auto it = st.pathstats[n].begin(); it != st.pathstats[n].end(); it++) {
      out << n << "," << it->path_length() << "," << it->nn_distance() << "," << it->wander_distance() << endl;
    }
  }
};

int main() {
  TestLineDistance();
  TestPathStatistics();
  int max_n = 5000;
  int iters = 5000;
  SimTable st(max_n);
  for(int i= 0; i < iters; i++)
    RGG(max_n,10,50, st);
  dump_all(cout, st);
  //int n = max_n - 10;
  //  dump_table_at(cout, st, n);
  return 0;
}
