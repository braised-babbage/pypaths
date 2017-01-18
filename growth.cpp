#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using std::cout;
using std::endl;
using std::abs;
using std::max;
using std::vector;
using std::ostream;

const bool DUMP_INFO = false;
const bool DUMP_BALL = false;


double eps(int n, double exponent) {
  return 1.0 / pow(n, exponent);
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
  out << "n,num_hops,path_length,nn_distance,wander_distance" << endl;
  for (int n = 0; n < count.size(); n++) {
    if (count[n] == 0)
      continue;

    for (auto it = pathstats[n].begin(); it != pathstats[n].end(); it++) {
      out << n << ","
	  << it->num_hops() << ","
	  << it->path_length() << ","
	  << it->nn_distance() << ","
	  << it->wander_distance() << endl;
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




void RGG(int n, int interval, int initial, double exponent,
	 StatsHandler& sh) {
  std::uniform_real_distribution<double> unif(-0.5,0.5);
  std::default_random_engine re;
  std::random_device rd;
  re.seed(rd());
  
  GeometricGraph g(eps(initial,exponent));
 
  for (int i = 0; i <= n; i++) {
    double x = unif(re);
    double y = unif(re);
    g.add_vertex(Point2D(x,y));
    
    if (i > initial && i % interval == 0) {
      g.shrink(eps(i,exponent));
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
  double eps_exp = 0.4;
  int max_n = 3000;
  int iters = 100;
  Point2D pa(-0.25,-0.25);
  Point2D pb(0.25,0.25);

  SimTable st(max_n,pa,pb);
  //  BallTracker bt(Point2D(0,0),0.25);
  for(int i= 0; i < iters; i++)
    RGG(max_n,10,50,eps_exp,st);

  cout << "max_n = " << max_n << "iters = " << iters
       <<" eps_exp = " << eps_exp
       << "from (" << pa.x << "," << pa.y << ")"
       << "to (" << pb.x << "," << pb.y << ")" << endl;
  st.dump_all(cout);

  
  //int n = max_n - 10;
  //  dump_table_at(cout, st, n);
  return 0;
}


// to just dump at the end, set interval to # iters
