#ifndef statistics_hpp
#define statistics_hpp



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
    return std::abs(nx*p.x + ny*p.y - c); 
  }
  
private:
  double nx, ny;
  double c;
};

class PathStatistics {
public:
  PathStatistics(Point2D pa, Point2D pb, const GeometricGraph& g);

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
  BallStatistics(const GeometricGraph& g, Point2D pa, double r);

  const std::vector<Point2D> ball() const {
    return points;
  }

private:
  std::vector<Point2D> points;

};

#endif
