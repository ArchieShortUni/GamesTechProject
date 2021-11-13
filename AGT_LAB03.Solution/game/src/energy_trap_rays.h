#pragma once
#include <engine.h>

class energy_trap_rays
{
public:
	energy_trap_rays(glm::vec3 p0, glm::vec3 dir);
	~energy_trap_rays();


	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate(glm::vec3 position, glm::vec3 direction);

	static engine::ref<energy_trap_rays> create(glm::vec3 p0, glm::vec3 dir);
	void swap() { if (s_active) { s_active = false; } else { s_active = true; } }

private:
	void render_line_segment(glm::vec3 p0, glm::vec3 p1, float alpha, glm::vec3 colour);
	void compute_path();

	engine::ref<engine::mesh> m_line;

	bool s_active;


	glm::vec3 m_position;

	glm::vec3 m_direction;
};
