#ifndef PLANETS_HPP
#define PLANETS_HPP

#include <memory>
#include <map>
#include <glbinding/gl/gl.h>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// use gl definitions from glbinding 
using namespace gl;

struct Planet{
  Planet(glm::fvec3 rotation = glm::fvec3(), glm::fvec3 translation = glm::fvec3(), glm::fvec3 scale = glm::fvec3(), double rotationSpeed = 1.0f, glm::vec3 color = glm::fvec3(), float glossyness = 1.0f, std::shared_ptr<Planet> ref_pl = nullptr)
  {
    rotation_ = rotation;
    translation_ = translation;
    scale_ = scale;
    rotationSpeed_ = rotationSpeed;
    color_ = color;
    ref_pl_ = ref_pl;
    glossyness_ = glossyness;

  }

  glm::fvec3 rotation_;
  glm::fvec3 translation_;
  glm::fvec3 scale_;
  double rotationSpeed_;
  glm::fvec3 color_;
  std::shared_ptr<Planet> ref_pl_;
  float glossyness_;

};

glm::fmat4 model_matrix(std::shared_ptr<Planet> const& planet)
{
  glm::fmat4 matrix{};

  if(planet->ref_pl_ != nullptr)
  {
    matrix *= model_matrix(planet->ref_pl_);
  }

  matrix *= glm::rotate(glm::fmat4{}, float(glfwGetTime()*planet->rotationSpeed_), planet->rotation_);
  matrix *= glm::translate(glm::fmat4{}, planet->translation_);
  
  return matrix;
}
#endif