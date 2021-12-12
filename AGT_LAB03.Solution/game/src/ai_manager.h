#pragma once
#include "behaviourTree.h"
#include "turret.h"
#include "enemy_ranged.h"
#include "enemy_engineer.h"
#include "enemy_melee.h"
#include "explosion.h"
#include "player.h"

class  ai_manager {
public:
	ai_manager(std::vector<engine::ref<engine::game_object>>& game_objects,engine::ref<player>& p, std::vector<engine::ref<beacon_switch>>& switches, engine::ref<engine::audio_manager>& audio);
	~ai_manager() {}

	static engine::ref<ai_manager> create(std::vector<engine::ref<engine::game_object>>& game_objects, engine::ref<player>& p, std::vector<engine::ref<beacon_switch>>& switches, engine::ref<engine::audio_manager>& audio) {
		return std::make_shared<ai_manager>(game_objects,p,switches,audio);
	}

	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);



	std::vector<engine::ref<enemy_ranged>>& get_enemies() { return active_ranged_enemies; }



	void add_enemy(game_enums::enemies enemyType ,glm::vec3 pos);


private:
	engine::ref<engine::audio_manager>& m_audio_manager;
	//Switches to pass through for the engineers to use
	std::vector<engine::ref<beacon_switch>> level_switches;
	//All enemies in the level to be updated and rendered 
	std::vector<engine::ref<turret>> active_turrets{};
	std::vector<engine::ref<enemy_ranged>> active_ranged_enemies{};
	std::vector<engine::ref<enemy_melee>> active_melee_enemies{}; 
	std::vector<engine::ref<enemy_engineer>> active_engineer_enemies{};
	//To add gameobjects to 
	std::vector<engine::ref<engine::game_object>>& m_game_objects;
	engine::ref<player>& target;


	engine::ref<explosion>	m_explosion{};
};
