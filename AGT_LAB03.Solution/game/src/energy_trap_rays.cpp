#include "pch.h"
#include "energy_trap_rays.h"

energy_trap_rays::energy_trap_rays(glm::vec3 p0, glm::vec3 dir)
{
	m_position = p0;
	m_direction = dir;
	s_active = true;
}

energy_trap_rays::~energy_trap_rays()
{}

void energy_trap_rays::compute_path()
{
	std::vector<engine::mesh::vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	m_vertices.push_back(engine::mesh::vertex(m_position, glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.0f)));
	glm::vec3 d = m_direction;
	for (int i = 1; i < 10; i++) {
		int hi = rand();
		float r1 = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		float r2 = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		float r3 = (rand() / (float)RAND_MAX) * 1.0f - 1.0f;
		float r4 = (rand() / (float)RAND_MAX) * .10f - 1.0f;

		glm::vec3 p = m_vertices[i - 1].position + 1.5f * m_direction * (1 + r4) + glm::vec3(r1, r2 / 1.5f, r3) * .25f;
		m_vertices.push_back(engine::mesh::vertex(p, glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.0f)));

		m_indices.push_back(i);
		if (i + 1 < 10)
			m_indices.push_back(i + 1);

		d = m_vertices[i].position - m_vertices[i - 1].position;
		glm::normalize(d);
	}

	m_line = engine::mesh::create(m_vertices, m_indices);
}

void energy_trap_rays::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	compute_path();

	
}

void energy_trap_rays::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);

	engine::material material = engine::material(0.0f, glm::vec3(1.0f), glm::vec3(0.f), glm::vec3(0.f), 0.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", glm::mat4(1.0f));

	for (int i = 1; i < 10; i += 2) {
		engine::renderer_api::line_width(i);
		float f = i / 10.0f;
		material.set_transparency(0.3f * (1 - f));
		material.submit(shader);

		m_line->va()->bind();
		engine::renderer_api::draw_indexed_lines(m_line->va());
	}

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);


}

void energy_trap_rays::activate(glm::vec3 position, glm::vec3 direction)
{
	s_active = true;
	m_position = position;
	m_direction = direction;
}

engine::ref<energy_trap_rays> energy_trap_rays::create(glm::vec3 p0, glm::vec3 dir)
{
	return std::make_shared<energy_trap_rays>(p0, dir);
}
