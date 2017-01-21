#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cassert>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using namespace std;

LineDistance::LineDistance(Point a, Point b) : o(0), v(0)  {
    double d = dist(a,b);
    o = a;
    v = b-o;
    v = v*(1/v.norm());
  }

double LineDistance::operator()(Point p) {
    auto u = p-o;
    double alpha = dot(u,v);
    auto w = u - (v*alpha);
    return w.norm();
}



PathStatistics::PathStatistics(Point pa, Point pb, const GeometricGraph& g)
{
    vertex_id a = g.closest(pa);
    vertex_id b = g.closest(pb);
    
    Point nna = g.position(a);
    Point nnb = g.position(b);

    ShortestPaths sp(g,a);

    nna_dist = dist(pa,nna);
    nnb_dist = dist(pb,nnb);
    
    length = sp.dist(b) + nn_distance();


    LineDistance ld(pa,pb);
    wander = max(ld(nna), ld(nnb));
    auto path = sp.pathTo(b);
    // the number of hops includes the initial and terminal segments,
    // but path does not store this information.
    hops = path.size() + 2;
    for (auto l = path.begin(); l != path.end(); l++) {
      wander = max(wander, ld(g.position(l->to)));
    }

    vmax = g.V();
}



BallStatistics::BallStatistics(const GeometricGraph& g, Point pa, double r)
{
  vertex_id a = g.closest(pa);
  Point nna = g.position(a);
  double nna_dist = dist(pa,nna);
  ShortestPaths sp(g,a);
  for (auto i = 0; i < g.V(); i++) {
    if (sp.dist(i)+nna_dist < r)
      points.push_back(g.position(i));
  }
}

void SimTable::update(const GeometricGraph& g, int n) {
  PathStatistics ps(pa, pb, g);
  if (ps.path_length() != M) { // avoid degenerate cases (TODO: exceptions)
    add_item(n, ps);
  }
}

void SimTable::add_item(int n, PathStatistics& ps)  {
    // TODO: use exceptions
    assert(ps.path_length() != M);
    assert(ps.nn_distance() != M);
    assert(ps.wander_distance() != M);
    count[n] += 1;
    pathstats[n].push_back(ps);
}

void SimTable::dump_all(ostream& out) {
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


void BallTracker::update(const GeometricGraph& g, int n) {
  ns.push_back(n);
  BallStatistics bs(g,center,radius);
  balls.push_back(bs.ball());
}

void BallTracker::dump_all(ostream& out) {
  out << "n,x,y" << endl;
  for (int i = 0; i < ns.size(); i++) {
    for (auto it = balls[i].begin(); it != balls[i].end(); it++) {
      out << ns[i] << "," << *it << endl;
    }
  }
}
