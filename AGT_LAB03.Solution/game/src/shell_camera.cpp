#include "pch.h"
#include "shell_camera.h"
#include "quad.h"

shell_camera::shell_camera(float width, float height) {
	m_texture = engine::texture_2d::create("assets/textures/shell_overlay.bmp", true);
	m_quad = quad::create(glm::vec2(width, height));
	shell_active = false;

	engine::ref<engine::model> shell_model = engine::model::create("assets/models/static/shell.obj");

	shell_props.meshes = shell_model->meshes();
	shell_props.textures = shell_model->textures();
	float shell_scale = 3.f / glm::max(shell_model->size().x, glm::max(shell_model->size().y, shell_model->size().z));
	shell_scale /= 5;
	shell_props.position = { 0,-10,0 };
	shell_props.bounding_shape = shell_model->size() / 2.f * shell_scale;
	shell_props.rotation_amount = AI_DEG_TO_RAD(270);
	shell_props.scale = glm::vec3(shell_scale);


}

shell_camera::~shell_camera(){}

void shell_camera::on_render3d(engine::ref<engine::shader> shader) {
	for (int i = 0; i < m_shells_container.size(); i++) {

		glm::mat4 shell_transform(1.0f);
		shell_transform = glm::translate(shell_transform, m_shells_container.at(i)->position());
		shell_transform = glm::rotate(shell_transform, m_shells_container.at(i)->rotation_amount(), m_shells_container.at(i)->rotation_axis());
		shell_transform = glm::scale(shell_transform, m_shells_container.at(i)->scale());
		engine::renderer::submit(shader, shell_transform, m_shells_container.at(i));
	}
}

void shell_camera::on_render2d(engine::ref<engine::shader> shader) {

	if (!shell_active)
		return;

	

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);

	//const auto text_shader = engine::renderer::shaders_library()->get("text_2d");
	//std::string text = "viewing shell: " + std::to_string(current_shell) + " / " + std::to_string(m_shells_container.size());
	//std::string text = "viewing shell: ";
	//m_text_manager->render_text(text_shader, text, (float)engine::application::window().width() / 2 - 200.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
}

void shell_camera::swap() {
	if (shell_active) { shell_active = false; }
	else { shell_active = true; }
}

void shell_camera::add_shell(glm::vec3 pos) {
	engine::ref<engine::game_object> m_shell = engine::game_object::create(shell_props);
	m_shell->set_position(pos); 
	m_shells_container.emplace_back(m_shell);

	if (m_shells_container.size() > 3) { m_shells_container.erase(m_shells_container.begin()); }
	else { current_shell++; }
}

glm::vec3 shell_camera::cycle_pos(int direction) {
	if (direction == 1) {
		if (current_shell == m_shells_container.size()-1 ) { current_shell = 0; }
		else { current_shell++; }}
	else {
		if (current_shell == 0) { current_shell = m_shells_container.size() -1; }
		else { current_shell--; }}

	return m_shells_container.at(current_shell)->position();
}
engine::ref<shell_camera> shell_camera::create(float width, float height)
{
	return std::make_shared<shell_camera> (width, height);
}
