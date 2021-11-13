#include "pch.h"
#include "menu_handler.h"
#include "quad.h"

menu_handler::menu_handler(float width, float height) {
	m_texture = engine::texture_2d::create("assets/textures/menu_controls.bmp", true);
	m_quad = quad::create(glm::vec2(width, height));
	
	//TextForMenu
	std::vector<std::string> letterstoprow = { "m-10-C.obj","m-9-O.obj","m-8-V.obj","m-7-E.obj","m-6-R.obj","m-5-T.obj","m-0-C2.obj","m-1-R2.obj","m-4-A.obj","m-3-B.obj" };

	for (int i = 0; i < letterstoprow.size(); i++) {
		std::string path = "assets/models/static/Menu/" + letterstoprow.at(i);

		engine::ref<engine::model> letter_model = engine::model::create(path);
		engine::game_object_properties letter_props;
		letter_props.meshes = letter_model->meshes();
		letter_props.textures = letter_model->textures();
		float letter_scale = 3.f / glm::max(letter_model->size().x, glm::max(letter_model->size().y, letter_model->size().z));
		letter_props.position = { 3.0f, 2.0f, 3.5f };
		letter_props.bounding_shape = letter_model->size() / 2.f * letter_scale;
		letter_props.bounding_shape /= 4;
		letter_props.rotation_amount = AI_DEG_TO_RAD(270);
		letter_props.scale = glm::vec3(letter_scale);

		m_letter = engine::game_object::create(letter_props);
		m_menuitems.emplace_back(m_letter);

	}
	engine::ref<engine::model> crab_model = engine::model::create("assets/models/static/Menu/m-2-crab.obj");
	engine::game_object_properties crab_props;
	crab_props.meshes = crab_model->meshes();
	crab_props.textures = crab_model->textures();
	float crab_scale = 3.f / glm::max(crab_model->size().x, glm::max(crab_model->size().y, crab_model->size().z));
	crab_scale *= 2.5;
	crab_props.position = { -2.0f,2.0f,-17.0f };
	crab_props.bounding_shape = crab_model->size() / 2.f * crab_scale;
	crab_props.rotation_amount = AI_DEG_TO_RAD(270);
	crab_props.scale = glm::vec3(crab_scale);

	m_crab = engine::game_object::create(crab_props);
}

menu_handler::~menu_handler() {}

void menu_handler::on_render3d(engine::ref<engine::shader> shader) {

	if (!menu_active)
		return;

	for (int i = 0; i < m_menuitems.size(); i++) {
		glm::mat4 letter_transform(1.0f);
		letter_transform = glm::translate(letter_transform, m_menuitems.at(i)->position());
		letter_transform = glm::translate(letter_transform, -m_menuitems.at(i)->offset() * m_menuitems.at(i)->scale());
		letter_transform = glm::rotate(letter_transform, m_menuitems.at(i)->rotation_amount(), m_menuitems.at(i)->rotation_axis());
		letter_transform = glm::scale(letter_transform, m_menuitems.at(i)->scale());

		engine::renderer::submit(shader, letter_transform, m_menuitems.at(i));
	}

	glm::mat4 crab_transform(1.0f);
	crab_transform = glm::translate(crab_transform, m_crab->position());
	crab_transform = glm::rotate(crab_transform, m_crab->rotation_amount(), m_crab->rotation_axis());
	crab_transform = glm::scale(crab_transform, m_crab->scale());
	engine::renderer::submit(shader, crab_transform, m_crab);
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




engine::ref<menu_handler> menu_handler::create(float width, float height)
{
	return std::make_shared<menu_handler>(width, height);
}