#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"
#include "projectile.h"
#include "player.h"


class enemy_ranged {

	enum class state {

		idle,
		patrol,
		approach,
		shoot,
		stuck,
		attemptingUnstick
	};


public:
	enemy_ranged(glm::vec3 pos, float h, engine::ref<player> targ);
	~enemy_ranged() {};

	static engine::ref<enemy_ranged> create(glm::vec3 pos, float h, engine::ref<player> targ) {
		return std::make_shared<enemy_ranged>(pos, h, targ);
	}

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

	//engine::bounding_box get_hit_box() { return base_hitbox; }

	float get_health() { return health; }

	glm::vec3 get_pos() { return m_enemy->position(); }

	engine::ref<engine::game_object>& get_object() { return m_enemy; }

	engine::bounding_box get_hit_box() { return hitbox; }

	//AI STATE FUNCTIONS\\

	void patrol(const engine::timestep& time_step);

	void approach(const engine::timestep& time_step);

	void shoot(const engine::timestep& time_step);

	void generate_patrol_point(float radius, float min_range);

	void unstuck();
private:

	float health = 100;
	bool alive = true;
	engine::game_object_properties bl_props{};

	engine::game_object_properties enemy_props{};

	
	engine::ref<engine::game_object> m_enemy{};

	engine::ref<player>  target{};

	engine::bounding_box hitbox;

	float time_since_last_shot = 0.f;

	std::vector<engine::ref<projectile>> active_projectiles{};
	bool is_active = false;
	float last_angle = 0;

	

	//AI function variables\\

	float time_alive = 0;
	float last_second = 1;
	float time_since_stuck = 0;
	glm::vec3 previous_position = glm::vec3(0.f, 0.f, 0.f);

	state m_state = state::idle;
	state m_last_state;
	bool attempting_to_unstick;
	glm::vec3 stuck_point;

	float movement_speed = 2.5f;

	glm::vec3 patrol_point = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 temp_point;
	bool reached_patrol_point = true;
};
