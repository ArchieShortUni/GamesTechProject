#include "pch.h"
#include "menu_handler.h"
#include "quad.h"

menu_handler::menu_handler(float width, float height, engine::ref<engine::game_object>& bg): m_back_terrain(bg) {
	m_texture = engine::texture_2d::create("assets/textures/menu_controls.bmp", true);
	m_quad = quad::create(glm::vec2(width, height));


	m_pointLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_pointLight.AmbientIntensity = 1.f;
	m_pointLight.DiffuseIntensity = 1.f;
	m_pointLight.Position = glm::vec3(0.f, 10.f, 0.f);
	m_pointLight.Attenuation.Constant = 1.0f;
	m_pointLight.Attenuation.Linear = 0.1f;
	m_pointLight.Attenuation.Exp = 0.001f;

	//TextForMenu
	std::vector<std::string> letterstoprow = { "C.obj","O.obj","V.obj","E.obj","R.obj","T.obj","C.obj","R.obj","A.obj","B.obj" };
	//std::vector<std::string> letterstoprow = { "C.obj","O.obj" };

	for (int i = 0; i < letterstoprow.size(); i++) {
		std::string path = "assets/models/static/Menu/" + letterstoprow.at(i);
		glm::vec3 initial_pos = glm::vec3( 3.0f, 20.3f, -9.5f );

		initial_pos.z += i*3.6f;
		//initial_pos.z += i * 5.6f;
		if (i > 5){
			initial_pos.x -= 4.f;
			initial_pos.z -= 14.f;
		}

		engine::ref<engine::model> letter_model = engine::model::create(path);
		engine::game_object_properties letter_props;
		letter_props.meshes = letter_model->meshes();
		letter_props.textures = letter_model->textures();
		float letter_scale = 3.f / glm::max(letter_model->size().x, glm::max(letter_model->size().y, letter_model->size().z));
		letter_props.position = initial_pos;
		letter_props.bounding_shape = glm::vec3(.4f, .9f, .4f);
		letter_props.type = 0;
		letter_props.rotation_amount = AI_DEG_TO_RAD(270);
		letter_props.scale = glm::vec3(letter_scale);
		letter_props.mass = 1.f;

		engine::bounding_box hitbox;
		hitbox.set_box(letter_props.bounding_shape.x, letter_props.bounding_shape.y, letter_props.bounding_shape.z, letter_props.position);
		m_boxes.push_back(hitbox);

		m_letter = engine::game_object::create(letter_props);
		m_menuitems.emplace_back(m_letter);
	}

	for (int i = 0; i < 40; i++) {
		engine::ref<menu_enemy> me = menu_enemy::create(glm::vec3(10.0f, 0.5f, (-14.5f+(i*2.f))), 10.f);
		//engine::ref<menu_enemy> me = menu_enemy::create(glm::vec3(8.0f, 2.f, -9.5f), 10.f);
		m_enemies.push_back(me);
		m_menuitems.push_back(me->get_object());
	}

	for (int i = 0; i < 40; i++) {
		engine::ref<menu_enemy> me = menu_enemy::create(glm::vec3(-10.0f, 0.5f, (-14.5f + (i * 2.f))), 10.f);
		//engine::ref<menu_enemy> me = menu_enemy::create(glm::vec3(8.0f, 2.f, -9.5f), 10.f);
		m_enemies.push_back(me);
		m_menuitems.push_back(me->get_object());
	}

	m_menuitems.push_back(m_back_terrain);
	m_physics_manager = engine::bullet_manager::create(m_menuitems);

}

menu_handler::~menu_handler() {}

void menu_handler::on_render3d(engine::ref<engine::shader> shader) {

	if (!menu_active)
		return;

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("gNumPointLights", (int)num_point_lights);

	m_pointLight.submit(shader, 0);

	for (int i = 0; i < m_boxes.size(); i++) {
		//m_boxes.at(i).on_render(2.5f, 0.f, 0.f, shader);
	}

	for (int i = 0; i < m_enemies.size(); i++) {
		m_enemies.at(i)->on_render(shader);
	}

	for (int i = 0; i < m_menuitems.size(); i++) {
		glm::mat4 letter_transform(1.0f);
		letter_transform = glm::translate(letter_transform, m_menuitems.at(i)->position());
		//letter_transform = glm::translate(letter_transform, -m_menuitems.at(i)->offset() * m_menuitems.at(i)->scale());
		letter_transform = glm::rotate(letter_transform, m_menuitems.at(i)->rotation_amount(), m_menuitems.at(i)->rotation_axis());
		letter_transform = glm::scale(letter_transform, m_menuitems.at(i)->scale());

		engine::renderer::submit(shader, letter_transform, m_menuitems.at(i));
	}

}

void menu_handler::on_render2d(engine::ref<engine::shader> shader) {

	if (!menu_active)
		return;



	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);

	
}

void menu_handler::swap() {
	if (menu_active) { menu_active = false; }
	else { menu_active = true; }
}




engine::ref<menu_handler> menu_handler::create(float width, float height, engine::ref<engine::game_object>& bg)
{
	return std::make_shared<menu_handler>(width, height,bg);
}
void menu_handler::on_update(const engine::timestep& time_step) {
	for (int i = 0; i < m_enemies.size(); i++) {
		m_enemies.at(i)->on_update(time_step);
	}
	m_physics_manager->dynamics_world_update(m_menuitems, double(time_step));

	for (int i = 0; i < 9; i++) {
		glm::vec3 hitboxpos = m_menuitems.at(i)->position();
		hitboxpos.z += .3f;
		m_boxes.at(i).on_update(hitboxpos, m_menuitems.at(i)->rotation_amount(), m_menuitems.at(i)->rotation_axis());
	}
}
