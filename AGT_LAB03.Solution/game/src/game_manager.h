#pragma once
#include <engine.h>
#include "beacon.h"
#include "progress_bar.h"
#include "player.h"
#include "turret.h"
#include "explosion.h"
class quad;

class game_manager {
public:
	game_manager(engine::perspective_camera& camera, float width, float height);
	~game_manager() {};

	static engine::ref<game_manager>create(engine::perspective_camera& camera, float width, float height);

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

private:
	engine::ref<engine::bullet_manager> m_physics_manager{};
	std::vector<engine::ref<engine::game_object>>     m_game_objects{};
	
	engine::perspective_camera& cam;

	engine::ref<progress_bar> level_bar;
	engine::ref<progress_bar> beacon1_bar;
	engine::ref<progress_bar> beacon2_bar;
	engine::ref<progress_bar> beacon3_bar;
	engine::ref<progress_bar> health_bar;

	glm::vec3 bcolour1 = glm::vec3(.76f, .87f, .94f);
	glm::vec3 bcolour2 = glm::vec3(.18f, .52f, .54f);
	glm::vec3 bcolour3 = glm::vec3(.21f, .53f, .64f);

	float level_complete_percent; 
	float current_level_percent; 
	engine::ref<engine::text_manager>	m_text_manager{};

	std::vector<engine::ref<beacon>> level_beacons;
	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;

	engine::ref<explosion>	m_explosion{};

	engine::ref<player> player;
	glm::vec3 last_player_pos;
	

	float level_time = 0;
	//TESTING TO BE DELETED LATER
	float timer_test = 0;
	std::vector<engine::ref<turret>> active_turrets{};
	engine::ref<turret> t;
};
