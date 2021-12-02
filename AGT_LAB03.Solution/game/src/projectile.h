#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"

class projectile {
public:
	projectile(engine::game_object_properties props,float origin_type);
	~projectile() { m_projectile->~game_object(); };

	static engine::ref<projectile> create(engine::game_object_properties props, float origin_type);

	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);

	void fire(glm::vec3 origin, glm::vec3 direction, float speed);

	glm::vec3 last_position() const { return m_last_position; }

	engine::ref<engine::game_object>& get_projectile_object() { return m_projectile; }

	float get_time_alive() { return time_alive; }

	engine::bounding_box get_hitbox() { return projectile_hitbox; }


private:
	glm::vec3 m_last_position{ 0.f };
	engine::ref<engine::game_object> m_projectile;
	engine::bounding_box projectile_hitbox;

	float projectile_speed;
	float time_alive = 0;

};
