#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"
#include "projectile.h"
#include "player.h"
#include "behaviourTree.h"

#include "game_enums.h"

class menu_enemy {

public:
	menu_enemy(glm::vec3 pos, float h);
	~menu_enemy() {};

	static engine::ref<menu_enemy> create(glm::vec3 pos, float h) {
		return std::make_shared<menu_enemy>(pos, h);
	}

	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);


	engine::ref<engine::game_object>& get_object() { return m_enemy; }

	engine::bounding_box get_hit_box() { return hitbox; }

	//AI STATE FUNCTIONS\\

	void patrol(const engine::timestep& time_step);

	void approach(const engine::timestep& time_step);

	void generate_patrol_point(float radius, float min_range);

	void unstuck();

private:
	engine::game_object_properties bl_props{};
	engine::game_object_properties enemy_props{};
	engine::ref<engine::game_object> m_enemy{};
	engine::bounding_box hitbox;

	//AI function variables\\

	float time_alive = 0;
	float last_second = 1;
	float time_since_stuck = 0;
	glm::vec3 previous_position = glm::vec3(0.f, 0.f, 0.f);

	game_enums::state m_state = game_enums::state::patrol;
	game_enums::state m_last_state;

	bool attempting_to_unstick;
	glm::vec3 stuck_point;

	float movement_speed = 2.5f;

	glm::vec3 patrol_point = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 temp_point;
	bool reached_patrol_point = true;


	game_enums::state next_state = game_enums::state::patrol;

};
#pragma once
