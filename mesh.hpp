#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

using namespace std;

struct face_plane {
    vector<Point3D> points;

    face_plane(const struct face_plane &f) {
        points = f.points;
    }
    face_plane(const vector<Point3D> &ps) {
        points = ps;
    }
};

// A polygonal mesh.
class Mesh : public Primitive {
public:

  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;
  vector<struct face_plane> facePlanes;

  NonhierBox *aabb;
  Vector3D dimension;

  virtual Type get_type() const {
    return Primitive::MESH;
  }
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
