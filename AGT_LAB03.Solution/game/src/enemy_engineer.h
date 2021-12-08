#pragma once
#pragma once
#include <engine.h>
#include"engine/entities/bounding_box_bullet.h"
#include "projectile.h"
#include "player.h"
#include "behaviourTree.h"
#include "beacon.h"
#include "game_enums.h"
#include "turret.h"

class enemy_engineer {




public:
	enemy_engineer(glm::vec3 pos, float h, engine::ref<player> targ, std::vector<engine::ref<beacon_switch>>& switches, std::vector<engine::ref<turret>>& all_turrets, engine::ref<engine::audio_manager>& audio);
	~enemy_engineer() {};

	static engine::ref<enemy_engineer> create(glm::vec3 pos, float h, engine::ref<player> targ, std::vector<engine::ref<beacon_switch>>& switches, std::vector<engine::ref<turret>>& all_turrets, engine::ref<engine::audio_manager>& audio) {
		return std::make_shared<enemy_engineer>(pos, h, targ,switches,all_turrets,audio);
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

	void approach(glm::vec3 targetPos);

	void shoot();

	void generate_patrol_point(float radius, float min_range);

	void unstuck();

	void run_behaviour_tree() {
		behaviorTree.run();
	}


	
private:



	float health = 100;
	bool alive = true;
	engine::game_object_properties bl_props{};

	engine::game_object_properties enemy_props{};
	engine::ref<engine::audio_manager>& m_audio_manager;

	engine::ref<engine::game_object> m_enemy{};

	engine::ref<player>  target{};

	engine::bounding_box hitbox;

	float time_since_last_shot = 0.f;
	std::vector<engine::ref<beacon_switch>> level_switches;
	std::vector<engine::ref<projectile>> active_projectiles{};
	std::vector<engine::ref<turret>>& active_turrets;
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

	glm::vec3 closest_switch_pos = glm::vec3(0, 0, 0);
	int closest_switch_int;
	
	//Behaviour Tree Setup

		//Switch Handling AI Things
	bool any_switches_on();
	void closest_switch(bool on);
	void turn_off_closest_switch();
	bool next_to_on_switch();
	void place_turret(glm::vec3 pos);
	void flee();
	BehaviourTree behaviorTree;
	BehaviourTree::Selector selector[6];
	BehaviourTree::Sequence sequence[7];
	game_enums::state next_state = game_enums::state::idle;

	bool action_bool = true;
	bool player_in_range = false;
	bool player_out_sight = false;
	bool switches_on = true;
	bool at_switch = false;
	bool turret_not_placed = true;
	bool offensive_turret_ready = false;

	float time_since_last_turret = 0.f;
	//Decision Actions
	Action targetOutSight = Action(game_enums::state::decision, player_out_sight, next_state);
	Action targetInRange = Action(game_enums::state::decision, player_in_range, next_state);
	Action anySwitchesOn = Action(game_enums::state::decision, switches_on, next_state);
	Action atSwitch = Action(game_enums::state::decision, at_switch, next_state);
	Action turretNotPlaced = Action(game_enums::state::decision, turret_not_placed, next_state);
	Action offensiveTurretReady = Action(game_enums::state::decision, offensive_turret_ready, next_state); 
	//Execution Actions
	Action patrol_action = Action(game_enums::state::patrol, action_bool, next_state);
	Action shoot_action = Action(game_enums::state::shoot, action_bool, next_state);
	Action approach_switch_action = Action(game_enums::state::approach, action_bool, next_state);
	Action turn_off_Switch_action = Action(game_enums::state::turnOffSwitch, action_bool, next_state);
	Action becomeIdle_action = Action(game_enums::state::idle, action_bool, next_state); 
	Action placing_turret_action = Action(game_enums::state::placingTurret, action_bool, next_state);
	Action flee_action = Action(game_enums::state::flee, action_bool, next_state);
	Action placing_offensive_turret_action = Action(game_enums::state::placingOffensiveTurret, action_bool, next_state);  
};
