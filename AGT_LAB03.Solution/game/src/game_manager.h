#pragma once
#include <engine.h>
#include "beacon.h"
#include "progress_bar.h"
#include "player.h"
#include "turret.h"
#include "explosion.h"
#include "enemy_ranged.h"
#include "ai_manager.h"
#include "pick_up.h"
class quad;

class game_manager {
public:
	game_manager(engine::perspective_camera& camera, float width, float height, engine::ref<engine::audio_manager>& audio);
	~game_manager() {};

	static engine::ref<game_manager>create(engine::perspective_camera& camera, float width, float height, engine::ref<engine::audio_manager>& audio);

	void initialise();

	void on_render2d(engine::ref<engine::shader> shader);
	void on_render3d(engine::ref<engine::shader> shader);

	void on_update(const engine::timestep& time_step);

	void interactStart() { player->interact_true(); }
	void interactEnd() { player->interact_false(); }

	void start_crouch() { player->start_crouch(); }
	void end_crouch() { player->end_crouch(); }

	void start_sprint() { player->start_sprint(); }
	void end_sprint() { player->end_sprint(); }

	void add_to_game_objects(engine::ref < engine::game_object> obj) { m_game_objects.push_back(obj); }

	void spawnEntities();

	glm::vec3 getRandomPos(float radius, float min_range);

private:


	engine::ref<engine::audio_manager>&  m_audio_manager;

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<ai_manager> m_ai_manager{};
	std::vector<engine::ref<engine::game_object>>     m_game_objects{};
	
	engine::perspective_camera& cam;

	engine::ref<progress_bar> level_bar;
	engine::ref<progress_bar> beacon1_bar;
	engine::ref<progress_bar> beacon2_bar;
	engine::ref<progress_bar> beacon3_bar;
	engine::ref<progress_bar> health_bar;

	glm::vec3 bcolour1 = glm::vec3(1.f, 0, 0);
	glm::vec3 bcolour2 = glm::vec3(0, 1.f, 0);
	glm::vec3 bcolour3 = glm::vec3(0, 0, 1.f);

	bool level_complete = false;

	float level_complete_percent; 
	float current_level_percent; 
	engine::ref<engine::text_manager>	m_text_manager{};

	std::vector<engine::ref<pickup>> level_pickups;
	std::vector<engine::ref<beacon>> level_beacons;
	std::vector<engine::ref<beacon_switch>>  level_switches;
	std::vector<glm::vec3> spawn_origins;
	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;

	engine::ref<explosion>	m_explosion{};

	engine::ref<player> player;
	glm::vec3 last_player_pos;
	

	float level_time = 182;
	float check = 181;
	float spawn_interval = 5;
	//TESTING TO BE DELETED LATER

	engine::ref<enemy_ranged> enemy; 
	std::vector<engine::ref<enemy_ranged>> active_enemies{};


	float timer_test = 0;
	std::vector<engine::ref<turret>> active_turrets{};
	engine::ref<turret> t;
};
