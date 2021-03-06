#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

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



ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,planets{}
 ,stars{}
 ,num_stars{}
 ,sqo{}
{
	glEnable(GL_DEPTH_TEST);
  	glDepthFunc(GL_LESS);	
	initializeFramebuffer(600u,450u);
	createScreenQuad();
	initializeGeometry();
	initializeShaderPrograms();

	planets = create_scene();

}

void ApplicationSolar::render() const {

	glBindFramebuffer(GL_FRAMEBUFFER, fb_handle);

	glClearColor(0.0,0.0,0.0,0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shaders.at("star").handle);

  	//bind VAO of geometry
 	glBindVertexArray(star_object.vertex_AO);
  	//draw geometry
	glDrawElements(star_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	// define planets

	for(std::vector<std::shared_ptr<Planet>>::const_iterator i = planets.begin(); i != planets.end(); ++i)
  	{
    	uploadPlanetTransforms(*i, i-planets.begin());
  	}


//	glUseProgram(m_shaders.at("planet").handle);

	// bind the VAO to draw
//	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
//	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shaders.at("screenquad").handle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, to.handle);
	int color_location = glGetUniformLocation(m_shaders.at("screenquad").handle, "ColorTex");
	glUniform1i(color_location, 0);

	glBindVertexArray(sqo.vertex_AO);
	utils::validate_program(m_shaders.at("screenquad").handle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



	
}

void ApplicationSolar::updateView() {

	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	glUseProgram(m_shaders.at("planet").handle);

	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
										 1, GL_FALSE, glm::value_ptr(view_matrix));

	//change sun position
	glm::fvec4 sun_pos = glm::fvec4(0.0f,0.0f,0.0f,1.0f);
	sun_pos = view_matrix * sun_pos;
	auto loc = glGetUniformLocation(m_shaders.at("planet").handle,"SunPosition");
	glUniform3f(loc, sun_pos.x, sun_pos.y, sun_pos.z);

	//set shader to star shader
	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
	             1, GL_FALSE, glm::value_ptr(view_matrix));

	glUseProgram(m_shaders.at("planet").handle);
}

void ApplicationSolar::updateProjection() {

	initializeFramebuffer(600u,450u);
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

		updateShader(0);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {

		updateShader(1);
	}
	else if(key == GLFW_KEY_7 && action == GLFW_PRESS)
	{ 
	    if (lum == 0){
			lum = 1;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Lum");
			glUniform1ui(loc, lum);
	    }
	    else
	    {
			lum = 0;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Lum");
			glUniform1ui(loc, lum);
		}
	}
	else if(key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		if (hor == 0)
		{
			hor = 1;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Hor");
			glUniform1ui(loc, hor);
		}
		else
		{
			hor = 0;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Hor");
			glUniform1ui(loc, hor);
		}
	}
	else if(key == GLFW_KEY_9 && action == GLFW_PRESS)
	{ 
		if (vert == 0)
		{
			vert = 1;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Vert");
			glUniform1ui(loc, vert);
		}
		else
		{
			vert = 0;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Vert");
			glUniform1ui(loc, vert);
		}
	}
	else if(key == GLFW_KEY_0 && action == GLFW_PRESS)
	{ 
		if (gau == 0)
		{
			gau = 1;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Gau");
			glUniform1ui(loc, gau);
		}
		else
		{
			gau = 0;
			glUseProgram(m_shaders.at("screenquad").handle);
			auto loc = glGetUniformLocation(m_shaders.at("screenquad").handle, "Gau");
			glUniform1ui(loc, gau);
		}
	}
}

void ApplicationSolar::updateShader(uint shader) {

	auto a = glGetUniformLocation(m_shaders.at("planet").handle,"Shader");
	glUniform1ui(a, shader);
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


	// request uniform locations for shader program
	m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
	m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
	//m_shaders.at("planet").u_locs["PlanetColor"] = -1;
	m_shaders.at("planet").u_locs["Glossyness"] = -1;
	m_shaders.at("planet").u_locs["ColorTex"] = -1;



	m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
																					 m_resource_path + "shaders/star.frag"});
	//inizialise star shader
  	m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
                                          m_resource_path + "shaders/star.frag"});
  	//m_shaders.at("star").u_locs["ModelMatrix"] = -1;
  	m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

	m_shaders.emplace("screenquad", shader_program{m_resource_path + "shaders/screenquad.vert",m_resource_path + "shaders/screenquad.frag"});

}

void ApplicationSolar::uploadPlanetTransforms(std::shared_ptr<Planet> const& planet, int const& i) const{

	glUseProgram(m_shaders.at("planet").handle);

	glActiveTexture(GL_TEXTURE0+i);
	glBindTexture(planet->texObj_.target, planet->texObj_.handle);

	int color_sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "ColorTex");
	glUniform1i(color_sampler_location, i);

	glm::fmat4 matrix;

	matrix *= model_matrix(planet);


	matrix = glm::scale(matrix, planet->scale_);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),1, GL_FALSE, glm::value_ptr(matrix));


	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),1, GL_FALSE, glm::value_ptr(normal_matrix));

	auto loc = glGetUniformLocation(m_shaders.at("planet").handle,"PlanetColor");
	glUniform3f(loc, planet->color_.x,planet->color_.y, planet->color_.z); 


	glUniform1f(m_shaders.at("planet").u_locs.at("Glossyness"), planet->glossyness_); 


	    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL); 
}


// load models
void ApplicationSolar::initializeGeometry() {
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

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

	glEnableVertexAttribArray(2);
	// second attribute is 3 floats with no offset & stride
	glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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

void ApplicationSolar::initializeFramebuffer(unsigned int width, unsigned int height)
{

	GLuint rb_handle = 0;

	glGenRenderbuffers(1, &rb_handle);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);

	
	//Textures without colors
	glGenTextures(1, &to.handle);
	glBindTexture(GL_TEXTURE_2D, to.handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GLint(GL_RGB), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fb_handle);
  	glBindFramebuffer(GL_FRAMEBUFFER, fb_handle);

  	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, to.handle,0);
  	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb_handle);

  	GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
  	glDrawBuffers(1, draw_buffers);

  	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  	if (status != GL_FRAMEBUFFER_COMPLETE) {
    	std::cout << "Erorr in frame buffer" << std::endl;
  	}
  }

void ApplicationSolar::createScreenQuad(){
  
  std::vector<float> vertices {
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // v1
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // v2
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // v4
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f  // v3
         };
  
  model sqm = {vertices, model::POSITION | model::TEXCOORD};
  // generate vertex array object
  glGenVertexArrays(1, &sqo.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(sqo.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &sqo.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, sqo.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sqm.data.size(), sqm.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, sqm.vertex_bytes, sqm.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, sqm.vertex_bytes, sqm.offsets[model::TEXCOORD]);
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

	std::string tPath = m_resource_path + "textures/";

	auto tSun = utils::create_texture_object(texture_loader::file(tPath + "sunmap.png"));
	auto tMercury = utils::create_texture_object(texture_loader::file(tPath + "deathstar.png"));
	auto tVenus = utils::create_texture_object(texture_loader::file(tPath + "venusmap.png"));
	auto tEarth = utils::create_texture_object(texture_loader::file(tPath + "earthmap1k.png"));
	auto tMars = utils::create_texture_object(texture_loader::file(tPath + "marsmap1k.png"));
	auto tJupiter = utils::create_texture_object(texture_loader::file(tPath + "jupitermap.png"));
	auto tSaturn = utils::create_texture_object(texture_loader::file(tPath + "saturnmap.png"));
	auto tUranus = utils::create_texture_object(texture_loader::file(tPath + "uranusmap.png"));	
	auto tNeptune = utils::create_texture_object(texture_loader::file(tPath + "neptunemap.png"));
	auto tPluto = utils::create_texture_object(texture_loader::file(tPath + "moonmap1k.png"));
	auto tMoon = utils::create_texture_object(texture_loader::file(tPath + "moonmap1k.png"));
	auto tSky = utils::create_texture_object(texture_loader::file(tPath + "stars.png"));

	//create planets & moons

	std::shared_ptr<Planet> Sun = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.5, 0.5, 0.5}, 0.15f, glm::vec3{1.0, 0.5, 0.2}, 6.0f, nullptr,tSun);
	std::shared_ptr<Planet> Mercury = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.1, 0.1, 0.1}, 0.5f, glm::vec3{0.2, 0.2, 0.6}, 6.0f,Sun,tMercury);
	std::shared_ptr<Planet> Venus = std::make_shared <Planet>(glm::vec3{0.0f, 3.0f, 0.0f}, glm::vec3{2.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 1.0f, glm::vec3{0.1, 0.5, 0.2}, 6.0f,Sun,tVenus);
	std::shared_ptr<Planet> Earth = std::make_shared <Planet>(glm::vec3{0.0f, 4.0f, 0.0f}, glm::vec3{3.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 0.9f, glm::vec3{0.7, 0.2, 0.4}, 6.5f, Sun,tEarth);
	std::shared_ptr<Planet> Mars = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{4.0f, 0.0f, 0.0f}, glm::vec3{0.09, 0.09, 0.09}, 0.8f, glm::vec3{0.8, 0.1, 0.7}, 6.0f, Sun,tMars);
	std::shared_ptr<Planet> Jupiter = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{5.0f, 0.0f, 0.0f}, glm::vec3{0.15, 0.15, 0.15}, 0.6f, glm::vec3{0.9, 0.1, 0.4}, 6.0f, Sun,tJupiter);
	std::shared_ptr<Planet> Saturn = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{6.0f, 0.0f, 0.0f}, glm::vec3{0.13, 0.13, 0.13}, 0.3f, glm::vec3{0.6, 0.9, 0.3}, 6.0f, Sun,tSaturn);
	std::shared_ptr<Planet> Uranus = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{7.0f, 0.0f, 0.0f}, glm::vec3{0.13, 0.13, 0.13}, 0.7f, glm::vec3{0.2, 0.1, 0.9}, 6.0, Sun,tUranus);
	std::shared_ptr<Planet> Neptune = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{8.0f, 0.0f, 0.0f}, glm::vec3{0.12, 0.12, 0.12}, 0.55f, glm::vec3{0.4, 0.8, 0.9}, 6.0f, Sun,tNeptune);
	std::shared_ptr<Planet> Pluto = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{9.0f, 0.0f, 0.0f}, glm::vec3{0.05, 0.05, 0.05}, 0.7f, glm::vec3{0.5, 0.5, 0.5}, 6.0f, Sun,tPluto);
	std::shared_ptr<Planet> Moon = std::make_shared <Planet>(glm::vec3{0.0f, 3.0f, 0.0f}, glm::vec3{0.5f, 0.0f, 0.0f}, glm::vec3{0.03, 0.03, 0.03}, 0.4f, glm::vec3{0.4, 0.4, 0.4}, 6.0f, Earth,tMoon);
	std::shared_ptr<Planet> Sky = std::make_shared <Planet>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{20.0, 20.0, 20.0}, 0.0f, glm::vec3{1.0,1.0,1.0}, 6.0f, nullptr,tSky);
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
	solarSystem.push_back(Sky);

	return solarSystem;
}

// exe entry point
int main(int argc, char* argv[]) {

	
	Launcher::run<ApplicationSolar>(argc, argv);
}
