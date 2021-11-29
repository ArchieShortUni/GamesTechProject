#pragma once
#include <engine.h>

class projectile {
public:
	projectile(const engine::perspective_camera& camera, float kick);
	~projectile();

	static engine::ref<projectile> create(const engine::perspective_camera& camera, float velocity);

	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);

	glm::vec3 last_position() const { return m_last_position; }
private:
	glm::vec3 m_last_position{ 0.f };
	engine::ref<engine::game_object> m_projectile; 
};
