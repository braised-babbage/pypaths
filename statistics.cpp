#include <cmath>
#include <algorithm>
#include <vector>
#include "geometric_graph.hpp"
#include "shortest_paths.hpp"
#include "statistics.hpp"

using std::vector;
using std::abs;
using std::max;


PathStatistics::PathStatistics(Point2D pa, Point2D pb, const GeometricGraph& g)
{
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
    // the number of hops includes the initial and terminal segments,
    // but path does not store this information.
    hops = path.size() + 2;
    for (auto l = path.begin(); l != path.end(); l++) {
      wander = max(wander, ld(g.position(l->to)));
    }

    vmax = g.V();
}



BallStatistics::BallStatistics(const GeometricGraph& g, Point2D pa, double r)
{
  vertex_id a = g.closest(pa);
  Point2D nna = g.position(a);
  double nna_dist = dist(pa,nna);
  ShortestPaths sp(g,a);
  for (auto i = 0; i < g.V(); i++) {
    if (sp.dist(i)+nna_dist < r)
      points.push_back(g.position(i));
  }
}
