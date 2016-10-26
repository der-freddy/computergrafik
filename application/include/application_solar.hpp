#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include <memory>

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);

  void mouseCallback(double pos_x, double pos_y);
  // draw all objects
  void render() const;

  std::vector<std::shared_ptr<Planet>> create_scene() const;


  void uploadPlanetTransforms(std::shared_ptr<Planet> const& planet) const;


 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  std::vector<std::shared_ptr<Planet>> planets;

  std::vector<float> stars;
  std::vector<unsigned> num_stars;
};

#endif