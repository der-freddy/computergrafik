#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

bool cel = false;
std::string shadername = "planet";
ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,planets{}
 ,stars{}
 ,num_stars{}
{

	initializeGeometry();
	initializeShaderPrograms();

	planets = create_scene();

}

void ApplicationSolar::render() const {


	glUseProgram(m_shaders.at("star").handle);

  	//bind VAO of geometry
 	glBindVertexArray(star_object.vertex_AO);
  	//draw geometry
	glDrawElements(star_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	// define planets

	for(std::vector<std::shared_ptr<Planet>>::const_iterator i = planets.begin(); i != planets.end(); ++i)
  	{
    	uploadPlanetTransforms(*i);
  	}

	glUseProgram(m_shaders.at("planet").handle);

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	
}

void ApplicationSolar::updateView() {

	if(cel == false)
	{
		m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",m_resource_path + "shaders/simple.frag"});
	}
	else
	{
		m_shaders.emplace("planet_cel", shader_program{m_resource_path + "shaders/cel.vert",m_resource_path + "shaders/cel.frag"});	
	}

	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);

	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at(shadername).u_locs.at("ViewMatrix"),
										 1, GL_FALSE, glm::value_ptr(view_matrix));

	//change sun position
	glm::fvec4 sun_pos = glm::fvec4(0.0f,0.0f,0.0f,1.0f);
	sun_pos = view_matrix * sun_pos;
	auto loc = glGetUniformLocation(m_shaders.at(shadername).handle,"SunPosition");
	glUniform3f(loc, sun_pos.x, sun_pos.y, sun_pos.z);

	//set shader to star shader
	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
	             1, GL_FALSE, glm::value_ptr(view_matrix));

	glUseProgram(m_shaders.at(shadername).handle);
}

void ApplicationSolar::updateProjection() {
	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
										 1, GL_FALSE, glm::value_ptr(m_view_projection));

	glUseProgram(m_shaders.at("star").handle);

 	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));

 	glUseProgram(m_shaders.at("planet").handle);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
	updateUniformLocations();
	
	// bind new shader
	glUseProgram(m_shaders.at("planet").handle);
	
	updateView();
	updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.1f, -0.0f});
		updateView();
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.1f, 0.0f});
		updateView();
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
		updateView();
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
		updateView();
	}

	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
		updateView();
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
		updateView();
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		cel = false;
		updateView();
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		cel = true;
		updateView();
	}



}

void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
  m_view_transform = glm::rotate(m_view_transform, (float)(pos_y/1000), glm::fvec3(-1.0f,0.0f,0.0f));
  m_view_transform = glm::rotate(m_view_transform, (float)(pos_x/1000), glm::fvec3(0.0f,-1.0f,0.0f));
  updateView();

}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
	// store shader program objects in container
	
	m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",m_resource_path + "shaders/simple.frag"});
	m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/cel.vert",m_resource_path + "shaders/cel.frag"});


	// request uniform locations for shader program
	m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
	m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("planet").u_locs["PlanetColor"] = -1;
	m_shaders.at("planet").u_locs["Glossyness"] = -1;


	m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
																					 m_resource_path + "shaders/star.frag"});
	//inizialise star shader
  	m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
                                          m_resource_path + "shaders/star.frag"});
  	//m_shaders.at("star").u_locs["ModelMatrix"] = -1;
  	m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
}

void ApplicationSolar::uploadPlanetTransforms(std::shared_ptr<Planet> const& planet) const{

	glUseProgram(m_shaders.at("planet").handle);

	glm::fmat4 matrix;

	matrix *= model_matrix(planet);


	matrix = glm::scale(matrix, planet->scale_);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),1, GL_FALSE, glm::value_ptr(matrix));


	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),1, GL_FALSE, glm::value_ptr(normal_matrix));

	glUniform3f(m_shaders.at("planet").u_locs.at("PlanetColor"), planet->color_.x,planet->color_.y, planet->color_.z); 


	glUniform1f(m_shaders.at("planet").u_locs.at("Glossyness"), planet->glossyness_); 


	    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL); 
}


// load models
void ApplicationSolar::initializeGeometry() {
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

	// generate vertex array object
	glGenVertexArrays(1, &planet_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(planet_object.vertex_AO);

	// generate generic buffer
	glGenBuffers(1, &planet_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	// first attribute is 3 floats with no offset & stride
	glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	// second attribute is 3 floats with no offset & stride
	glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

	 // generate generic buffer
	glGenBuffers(1, &planet_object.element_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
	// configure currently bound array buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

	// store type of primitive to draw
	planet_object.draw_mode = GL_TRIANGLES;
	// transfer number of indices to model object 
	planet_object.num_elements = GLsizei(planet_model.indices.size());


	// STARS
	//rand num of stars
	const unsigned numstars = 5000+(std::rand()%(10000-5000+1));

	for(unsigned i=0;i<numstars;++i)
	{
		num_stars.push_back(i);
		// define pos via rand()
		for(unsigned j=0;j<3;++j){
	  		stars.push_back(static_cast <float>(10 + (rand() % 301- 200)));
		}
		// define color of stars
		for(int k = 0; k < 3; ++k){
	  		stars.push_back(0.5f);
		}
	}

	// generate vertex array object
	glGenVertexArrays(1, &star_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(star_object.vertex_AO);

	// generate generic buffer
	glGenBuffers(1, &star_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stars.size(), stars.data() , GL_STATIC_DRAW);

	//enable position on gpu
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6.0f, NULL);
	
	//enable colors on gpu
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6.0f, (GLvoid*)uintptr_t(sizeof(float)*3));

	
	glGenBuffers(1, &star_object.element_BO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * numstars, num_stars.data(), GL_STATIC_DRAW);


	//draw stars
	star_object.draw_mode = GL_POINTS;
	star_object.num_elements = GLsizei(numstars);
}

ApplicationSolar::~ApplicationSolar() {
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);

	//destroy
	glDeleteBuffers(1, &star_object.vertex_BO);
	glDeleteBuffers(1, &star_object.element_BO);
	glDeleteVertexArrays(1, &star_object.vertex_AO);
}

std::vector<std::shared_ptr<Planet>> ApplicationSolar::create_scene() const{
	//create planets & moons

	std::shared_ptr<Planet> Sun = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.5, 0.5, 0.5}, 0.15f, glm::vec3{1.0, 0.5, 0.2}, 1.0f, nullptr);
	std::shared_ptr<Planet> Mercury = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.1, 0.1, 0.1}, 0.5f, glm::vec3{0.2, 0.2, 0.6}, 1.0f,Sun);
	std::shared_ptr<Planet> Venus = std::make_shared <Planet>(glm::vec3{0.0f, 3.0f, 0.0f}, glm::vec3{2.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 1.0f, glm::vec3{0.1, 0.5, 0.2}, 1.0f,Sun);
	std::shared_ptr<Planet> Earth = std::make_shared <Planet>(glm::vec3{0.0f, 4.0f, 0.0f}, glm::vec3{3.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 0.9f, glm::vec3{0.7, 0.2, 0.4}, 0.5f, Sun);
	std::shared_ptr<Planet> Mars = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{4.0f, 0.0f, 0.0f}, glm::vec3{0.09, 0.09, 0.09}, 0.8f, glm::vec3{0.8, 0.1, 0.7}, 1.0f, Sun);
	std::shared_ptr<Planet> Jupiter = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{5.0f, 0.0f, 0.0f}, glm::vec3{0.15, 0.15, 0.15}, 0.6f, glm::vec3{0.9, 0.1, 0.4}, 1.0f, Sun);
	std::shared_ptr<Planet> Saturn = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{6.0f, 0.0f, 0.0f}, glm::vec3{0.13, 0.13, 0.13}, 0.3f, glm::vec3{0.6, 0.9, 0.3}, 1.0f, Sun);
	std::shared_ptr<Planet> Uranus = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{7.0f, 0.0f, 0.0f}, glm::vec3{0.13, 0.13, 0.13}, 0.7f, glm::vec3{0.2, 0.1, 0.9}, 1.0, Sun);
	std::shared_ptr<Planet> Neptune = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{8.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 0.55f, glm::vec3{0.4, 0.8, 0.9}, 1.0f, Sun);
	std::shared_ptr<Planet> Pluto = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{9.0f, 0.0f, 0.0f}, glm::vec3{0.05, 0.05, 0.05}, 0.7f, glm::vec3{0.5, 0.5, 0.5}, 1.0f, Sun);
	std::shared_ptr<Planet> Moon = std::make_shared <Planet>(glm::vec3{0.0f, 3.0f, 0.0f}, glm::vec3{0.5f, 0.0f, 0.0f}, glm::vec3{0.03, 0.03, 0.03}, 0.4f, glm::vec3{0.4, 0.4, 0.4}, 1.0f, Earth);

	//put planets in vector
	std::vector<std::shared_ptr<Planet>> solarSystem;

	solarSystem.push_back(Sun);
	solarSystem.push_back(Mercury);
	solarSystem.push_back(Venus);
	solarSystem.push_back(Earth);
	solarSystem.push_back(Mars);
	solarSystem.push_back(Jupiter);
	solarSystem.push_back(Saturn);
	solarSystem.push_back(Uranus);
	solarSystem.push_back(Neptune);
	solarSystem.push_back(Pluto);
	solarSystem.push_back(Moon);

	return solarSystem;

}

// exe entry point
int main(int argc, char* argv[]) {

	
	Launcher::run<ApplicationSolar>(argc, argv);
}
