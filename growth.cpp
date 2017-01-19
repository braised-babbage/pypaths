#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using namespace std;


const bool DUMP_INFO = false;
const bool DUMP_BALL = false;


double eps(int n, double exponent) {
  return 1.0 / pow(n, exponent);
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
    g.add_vertex(Point(x,y));
    
    if (i > initial && i % interval == 0) {
      g.shrink(eps(i,exponent));
      sh.update(g,i);
    }
  }
}

// void dump_vertices(ostream& out, const GeometricGraph& g,
// 		   const vector<vertex_id> verts) {
//   for (auto it = verts.begin(); it != verts.end(); it++) {
//     Point p = g.position(*it);
//     out << p.x << " " << p.y << " ";
//   }
//   out << endl;
  
// }

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
  int max_n = 1000;
  int iters = 10;
  Point pa(-0.25,-0.25);
  Point pb(0.25,0.25);

  SimTable st(max_n,pa,pb);
  //  BallTracker bt(Point(0,0),0.25);
  for(int i= 0; i < iters; i++)
    RGG(max_n,10,50,eps_exp,st);

  cout << "max_n = " << max_n << "iters = " << iters
       <<" eps_exp = " << eps_exp
       << "from " << pa
       << "to " << pb << endl;
  st.dump_all(cout);

  
  //int n = max_n - 10;
  //  dump_table_at(cout, st, n);
  return 0;
}


// to just dump at the end, set interval to # iters
