#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{

  Point3D firstPoint = m_verts[0];

  double minX = firstPoint[0];
  double maxX = firstPoint[0];
  double minY = firstPoint[1];
  double maxY = firstPoint[1];
  double minZ = firstPoint[2];
  double maxZ = firstPoint[2];

  for (unsigned int i = 0; i < m_verts.size(); i++) {
    Point3D p = m_verts[i];
    minX = min(p[0], minX);
    maxX = max(p[0], maxX);
    minY = min(p[1], minY);
    maxY = min(p[1], maxY);
    minZ = min(p[2], minZ);
    maxZ = min(p[2], maxZ);
  }

  Point3D minPoint = Point3D(minX, minY, maxZ);
  double size = max(max(maxX - minX, maxY - minY), maxZ - minZ);

  aabb = new NonhierBox(minPoint, size);
  dimension[0] = maxX - minX;
  dimension[1] = maxY - minY;
  dimension[2] = maxZ - minZ;

  for (unsigned int i = 0; i < m_faces.size(); i++) {
    Face f = m_faces[i];
    vector<Point3D> points;

    for (unsigned int j = 0; j < f.size(); j++) {
      Point3D p = m_verts[f[j]];
      points.push_back(p);
    }

    facePlanes.push_back(face_plane(points));
  }
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";

  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
