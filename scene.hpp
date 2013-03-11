#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

using namespace std;

class SceneNode {
public:
  SceneNode(const string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }

  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  list<SceneNode*> get_children() const {
    return m_children;
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  virtual bool is_geometry() const;

  virtual Primitive *get_primitive() const {
    return NULL;
  }

  string getName() const;
protected:

  // Useful for picking
  int m_id;
  string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };


protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const Material* get_material() const;

  void set_material(Material* material)
  {
    m_material = material;
  }

  virtual Primitive* get_primitive() const {
    return m_primitive;
  }

  Primitive::Type get_type() const {
    return m_primitive->get_type();
  }

  virtual bool is_geometry() const;
protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
