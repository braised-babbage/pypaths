#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <tclap/CmdLine.h>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using namespace std;


const bool DUMP_INFO = false;
const bool DUMP_BALL = false;


double eps(int n, double exponent) {
  return 1.0 / pow(n, exponent);
};

uniform_real_distribution<double> unif(-0.5,0.5);
default_random_engine re;
random_device rd;

Point equal_coords(int d, double c) {
  Point p(0);
  for (int i = 0; i < d; i++) {
    p.coords.push_back(c);
  }
  return p;
}


Point random_point(int d) {
  // TODO: move this? it looks under the hood...
  Point p(0);
  for (int i = 0; i < d; i++) {
    p.coords.push_back(unif(re));
  }
  return p;
}


void RGG(int d, int n, int interval, int initial, double exponent,
	 vector<Point> starting, StatsHandler& sh) {
  GeometricGraph g(eps(initial,exponent));
  for (auto it = starting.begin(); it != starting.end(); it++) {
    g.add_vertex(*it);
  }
  for (int i = 0; i <= n; i++) {
    Point p = random_point(d);
    g.add_vertex(p);
    
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


void run_simulation(double eps_exp, int iters, int d,
		    int start_n, int max_n, int step) {
  Point pa = equal_coords(d,-0.25);
  Point pb = equal_coords(d, 0.25);
  vector<Point> starting;
  starting.push_back(pa); starting.push_back(pb);
  
  SimTable st(max_n,pa,pb);
  //  BallTracker bt(Point(0,0),0.25);
  for(int i= 0; i < iters; i++)
    RGG(d,max_n,step,start_n,eps_exp,starting,st);
  
  st.dump_all(cout);

}



int main(int argc, char** argv) {
  try {
    TCLAP::CmdLine cmd("RGG Growth Sim", ' ', "0.1");
    TCLAP::ValueArg<int> d("d", "dim", "dimension", false, 2, "int");
    TCLAP::ValueArg<double> exp("e", "exp", "Epsilon Exponent",
				true, 0.0, "double");
    TCLAP::ValueArg<int> max("n", "max", "Max n", true, 0, "int");
    TCLAP::ValueArg<int> start("s", "start", "Starting n", true, 0, "int");
    TCLAP::ValueArg<int> step("z", "stepsize", "Step Size (n)", false,10,"int");
    TCLAP::ValueArg<int> iters("i", "iters", "Iterations", false, 10,"int");
    cmd.add(iters);  cmd.add(step);  cmd.add(start);
    cmd.add(max);    cmd.add(exp);   cmd.add(d);
    cmd.parse(argc, argv);

    cout << "growth --dim " << d.getValue()
	 << " --exp " << exp.getValue()
	 << " --max " << max.getValue()
	 << " --start " << start.getValue()
	 << " --stepsize " << step.getValue()
	 << " --iters " << iters.getValue()
	 << endl;
    run_simulation(exp.getValue(), iters.getValue(), d.getValue(),
		   start.getValue(), max.getValue(), step.getValue());
  } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl; } 
  return 0;
}


// to just dump at the end, set interval to # iters
