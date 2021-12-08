#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"
#include "projectile.h"
#include "player.h"
#include "behaviourTree.h"

#include "game_enums.h"

class enemy_ranged {

	


public:
	enemy_ranged(glm::vec3 pos, float h, engine::ref<player> targ, bool hasShotgun);
	~enemy_ranged() {};

	static engine::ref<enemy_ranged> create(glm::vec3 pos, float h, engine::ref<player> targ, bool hasShotgun) {
		return std::make_shared<enemy_ranged>(pos, h, targ,hasShotgun);
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

	void shoot();

	void generate_patrol_point(float radius, float min_range);


	void run_behaviour_tree() {
		behaviorTree.run();
	}

private:
	bool shotgun = false;
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

	game_enums::state m_state = game_enums::state::idle;
	game_enums::state m_last_state;

	bool attempting_to_unstick;
	glm::vec3 stuck_point;

	float movement_speed = 2.5f;

	glm::vec3 patrol_point = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 temp_point;
	bool reached_patrol_point = true;

	//Behaviour Tree Setup
	BehaviourTree behaviorTree;
	BehaviourTree::Selector selector[2];
	BehaviourTree::Sequence sequence[2];
	game_enums::state next_state = game_enums::state::idle; 

	bool action_bool = true;
	bool player_in_range = false;
	bool player_in_sight = false;

	//Decision Actions
	Action targetInSight = Action(game_enums::state::decision, player_in_sight, next_state);
	Action targetInRange = Action(game_enums::state::decision, player_in_range, next_state);
	//Execution Actions
	Action patrol_action = Action(game_enums::state::patrol, action_bool, next_state);
	Action shoot_action = Action(game_enums::state::shoot, action_bool, next_state);
	Action approach_action = Action(game_enums::state::approach, action_bool, next_state);


};
