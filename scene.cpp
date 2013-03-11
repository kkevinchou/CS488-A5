#include "scene.hpp"
#include "a2.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
    Matrix4x4 t = rotation(angle, axis);
    Matrix4x4 matrix = m_trans * t;
    set_transform(matrix);
}

void SceneNode::scale(const Vector3D& amount)
{
    Matrix4x4 t = scaling(amount);
    Matrix4x4 matrix = m_trans * t;
    set_transform(matrix);
}

void SceneNode::translate(const Vector3D& amount)
{
    Matrix4x4 t = translation(amount);
    Matrix4x4 matrix = m_trans * t;
    set_transform(matrix);
}

bool SceneNode::is_joint() const
{
  return false;
}

bool SceneNode::is_geometry() const {
  return false;
}

string SceneNode::getName() const {
  return m_name;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::is_geometry() const {
  return true;
}

const Material *GeometryNode::get_material() const {
  return m_material;
}