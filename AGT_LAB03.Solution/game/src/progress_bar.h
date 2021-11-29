#pragma once
#include <engine.h>
#include "quad.h"

class quad;

class progress_bar {

public:
	progress_bar(float height, float width, glm::vec2 position, float max_v, const std::string& path_back, const std::string& path_front, const std::string& path_bar_texture) {
		pos = position;
		m_back_quad = quad::create(glm::vec2(width, height));
		m_texture_back = engine::texture_2d::create(path_back, true);

		float front_x = width - .01f;
		float front_y = height - .01f;
		m_front_quad = quad::create(glm::vec2(front_x, front_y));
		m_texture_front = engine::texture_2d::create(path_front, true);

		m_front_quad->reset();
		m_front_quad->change_percent(50);

		m_texture_quad = quad::create(glm::vec2(front_x, front_y));
		m_texture_middle = engine::texture_2d::create(path_bar_texture, true);

		

	}
	~progress_bar(){}


	static engine::ref<progress_bar> progress_bar::create(float height, float width, glm::vec2 position, float max_v, const std::string& path_back, const std::string& path_front, const std::string& path_bar_texture)
	{
		return std::make_shared<progress_bar>( height,  width,  position,  max_v, path_back, path_front, path_bar_texture);
	}

	void update_bar(int percentage) {
		m_front_quad->change_percent(percentage);
	}

	void on_update(const engine::timestep& time_step){}

	void on_render2d(engine::ref<engine::shader> shader){
		glm::mat4 transform_back(1.0f);
		transform_back = glm::translate(transform_back, glm::vec3(pos.x, pos.y, 0.1f));

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		m_texture_back->bind();
		engine::renderer::submit(shader, m_back_quad->mesh(), transform_back);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);


		glm::mat4 transform_texture(1.f);
		transform_texture = glm::translate(transform_texture, glm::vec3(pos.x, pos.y, 0.2f));

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		m_texture_middle->bind();
		engine::renderer::submit(shader, m_texture_quad->mesh(), transform_texture);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);

		glm::mat4 transform_front(1.0f);
		transform_front = glm::translate(transform_front, glm::vec3(pos.x, pos.y, 0.3f));

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		m_texture_front->bind();
		engine::renderer::submit(shader, m_front_quad->mesh(), transform_front);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);

	}

private:

	glm::vec2 pos;

	engine::ref<quad> m_back_quad;
	engine::ref<quad> m_front_quad;
	engine::ref<quad> m_texture_quad;
	engine::ref<engine::texture_2d> m_texture_back;
	engine::ref<engine::texture_2d> m_texture_front;
	engine::ref<engine::texture_2d> m_texture_middle; 


};
