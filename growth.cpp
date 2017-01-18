#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <algorithm>
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
const bool DUMP_BALL = false;


double eps(int n, double exponent = 0.45) {
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

    nna_dist = dist(pa,nna);
    nnb_dist = dist(pb,nnb);
    
    length = sp.dist(b) + nn_distance();

    LineDistance ld(pa,pb);
    wander = max(ld(nna), ld(nnb));
    auto path = sp.pathTo(b);
    for (auto l = path.begin(); l != path.end(); l++) {
      wander = max(wander, ld(g.position(l->to)));
    }

    vmax = g.V();
  };

  double nn_distance() {
    return nna_dist + nnb_dist;
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
  double nna_dist, nnb_dist;
  vertex_id vmax;
};

class BallStatistics {
public:
  BallStatistics(const GeometricGraph& g, Point2D pa, double r) {
    vertex_id a = g.closest(pa);
    Point2D nna = g.position(a);
    double nna_dist = dist(pa,nna);
    ShortestPaths sp(g,a);
    for (auto i = 0; i < g.V(); i++) {
      if (sp.dist(i)+nna_dist < r)
	points.push_back(g.position(i));
    }
  }

  const vector<Point2D> ball() const {
    return points;
  }

private:
  vector<Point2D> points;

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
  assert(ps.path_length() == 4);
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



class StatsHandler {
public:
  virtual void update(const GeometricGraph& g, int n) = 0;
};


class SimTable : public StatsHandler {
public:
  SimTable(int max_n, Point2D pa, Point2D pb)
    : pa(pa), pb(pb), count(max_n+1,0), pathstats(max_n+1, vector<PathStatistics>())
  { }

  void update(const GeometricGraph& g, int n) {
    PathStatistics ps(pa, pb, g);
    if (ps.path_length() != M) { // avoid degenerate cases (TODO: exceptions)
      add_item(n, ps);
    }
  }

  void add_item(int n, PathStatistics& ps) {
    // use exceptions
    assert(ps.path_length() != M);
    assert(ps.nn_distance() != M);
    assert(ps.wander_distance() != M);
    count[n] += 1;
    pathstats[n].push_back(ps);
  }

  void dump_all(ostream& out) {
  //  out << "eps = " << 0.4 << endl;
    // this is too tightly couples to the PathStatistics class
  out << "n,path_length,nn_distance,wander_distance" << endl;
  for (int n = 0; n < count.size(); n++) {
    if (count[n] == 0)
      continue;

    for (auto it = pathstats[n].begin(); it != pathstats[n].end(); it++) {
      out << n << "," << it->path_length() << "," << it->nn_distance() << "," << it->wander_distance() << endl;
    }
  }
};

private:
  vector<int> count;
  vector<vector<PathStatistics> > pathstats;

  Point2D pa, pb;
};

class BallTracker : public StatsHandler {
public:
  BallTracker(Point2D center, double radius) : center(center), radius(radius) {
    
  }

  void update(const GeometricGraph& g, int n) {
    ns.push_back(n);
    BallStatistics bs(g,center,radius);
    balls.push_back(bs.ball());
  }

  void dump_all(ostream& out) {
    out << "n,x,y" << endl;
    for (int i = 0; i < ns.size(); i++) {
      for (auto it = balls[i].begin(); it != balls[i].end(); it++) {
	out << ns[i] << "," << it->x << "," << it->y << endl;
      }
    }
  }

private:
  Point2D center;
  vector<vector<Point2D> > balls;
  vector<int> ns;
  double radius;

};




void RGG(int n, int interval, int initial, StatsHandler& sh) {
  std::uniform_real_distribution<double> unif(-0.5,0.5);
  std::default_random_engine re;
  std::random_device rd;
  re.seed(rd());
  
  GeometricGraph g(eps(initial));
 
  for (int i = 0; i <= n; i++) {
    double x = unif(re);
    double y = unif(re);
    g.add_vertex(Point2D(x,y));
    
    if (i > initial && i % interval == 0) {
      g.shrink(eps(i));
      sh.update(g,i);
    }
  }
}

void dump_vertices(ostream& out, const GeometricGraph& g,
		   const vector<vertex_id> verts) {
  for (auto it = verts.begin(); it != verts.end(); it++) {
    Point2D p = g.position(*it);
    out << p.x << " " << p.y << " ";
  }
  out << endl;
  
}

// void dump_table_at(ostream& out, SimTable& st, int n) {
//   out << "n = " << n << " eps_exponent = " << 0.4 << " iters = " << st.count[n] << endl;
//   out << "path_length nn_distance wander_distance" << endl;
//   for(auto it = st.pathstats[n].begin(); it != st.pathstats[n].end(); it++) {
//     out << it->path_length() << " " << it->nn_distance() << " " << it->wander_distance() << endl;
//   }
//   out << endl;
// }





int main() {
  TestLineDistance();
  TestPathStatistics();
  TestBallStatistics();
  int max_n = 5000;
  int iters = 500;
  Point2D pa(-0.25,-0.25);
  Point2D pb(0.25,0.25);

  SimTable st(max_n,pa,pb);
  //  BallTracker bt(Point2D(0,0),0.25);
  for(int i= 0; i < iters; i++)
    RGG(max_n,10,50, st);
  
  st.dump_all(cout);

  
  //int n = max_n - 10;
  //  dump_table_at(cout, st, n);
  return 0;
}


// to just dump at the end, set interval to # iters
