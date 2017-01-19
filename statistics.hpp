#ifndef statistics_hpp
#define statistics_hpp



class LineDistance {
public:
  LineDistance(Point pa, Point pb); 

  
  double operator()(Point p); 
  
private:
  Point o,v;
};

class PathStatistics {
public:
  PathStatistics(Point pa, Point pb, const GeometricGraph& g);

  double nn_distance() {
    return nna_dist + nnb_dist;
  }

  double path_length() {
    return length;
  }

  int num_hops() {
    return hops;
  }

  double wander_distance() {
    return wander;
  }

private:
  int hops;
  double length;
  double wander;
  double nna_dist, nnb_dist;
  vertex_id vmax;
};


class BallStatistics {
public:
  BallStatistics(const GeometricGraph& g, Point pa, double r);

  const std::vector<Point> ball() const {
    return points;
  }

private:
  std::vector<Point> points;

};

class StatsHandler {
public:
  virtual void update(const GeometricGraph& g, int n) = 0;
  virtual void dump_all(std::ostream& out) = 0;
};


class SimTable : public StatsHandler {
public:
  SimTable(int max_n, Point pa, Point pb)
    : pa(pa), pb(pb), count(max_n+1,0), pathstats(max_n+1, std::vector<PathStatistics>())
  { }

  void update(const GeometricGraph& g, int n);

  void add_item(int n, PathStatistics& ps);

  void dump_all(std::ostream& out);

private:
  std::vector<int> count;
  std::vector<std::vector<PathStatistics> > pathstats;

  Point pa, pb;
};

class BallTracker : public StatsHandler {
public:
  BallTracker(Point center, double radius) : center(center), radius(radius) {
    
  }

  void update(const GeometricGraph& g, int n);

  void dump_all(std::ostream& out);

private:
  Point center;
  std::vector<std::vector<Point> > balls;
  std::vector<int> ns;
  double radius;

};

#endif
