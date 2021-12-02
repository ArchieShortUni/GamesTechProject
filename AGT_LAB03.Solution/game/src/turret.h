#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"
#include "projectile.h"
#include "player.h"

class turret {
public:
	turret(glm::vec3 pos, float h, engine::ref<player> targ);
	~turret() {};

	static engine::ref<turret> create(glm::vec3 pos, float h, engine::ref<player> targ);

	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);

	void take_damage(float damage) {
		health -= damage;
		if (health <= 0) {
			health = 0;
			alive = false;
		}
	}
	bool is_alive() { return alive; }

	engine::bounding_box get_hit_box() { return base_hitbox; }

	float get_health() { return health; }

	glm::vec3 get_pos() { return m_turret->position(); }
private:

	float health = 100;
	bool alive = true;
	engine::game_object_properties bl_props{};

	engine::ref<engine::material> m_active_material;
	engine::ref<engine::material> m_disabled_material;

	engine::ref<engine::game_object> m_turret{};
	engine::ref<engine::game_object> m_base{};
	engine::ref<player>  target{};

	engine::bounding_box base_hitbox;

	float time_since_last_shot = 0.f;
	std::vector<engine::ref<projectile>> active_projectiles{};
	bool is_active = false;
	float last_angle = 0;
};
