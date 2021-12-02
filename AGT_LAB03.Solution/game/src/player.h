#pragma once
#include <engine.h>

#include"engine/entities/bounding_box_bullet.h"
#include "glm/gtx/rotate_vector.hpp"
#include "engine/core/input.h"
#include "engine/key_codes.h"
#include "projectile.h"
#include "quad.h"


class player {
public:

	player(engine::perspective_camera& camera, std::vector<engine::ref<engine::game_object>>& game_objs);
	~player(){}

	static engine::ref<player> create(engine::perspective_camera& camera, std::vector<engine::ref<engine::game_object>>& game_objs) {
		return std::make_shared<player>(camera, game_objs);
	}

	//void initialise(engine::perspective_camera& camera);

	void initialise(float width, float height);
	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);
	void on_render2d(engine::ref<engine::shader> shader);

	void update_player_data(glm::vec3 new_position);
	void update_camera();

	glm::vec3 get_player_position() {
		glm::vec3 position  =player_pos - (player_camera.front_vector()/glm::vec3(120.f));
		return position; }
	engine::bounding_box get_hitbox() { return player_hitbox;  }


	void interact_true(){
		interaction_box_active = true;
	}

	void interact_false() {
		interaction_box_active = false;
	}

	bool get_interaction() { return interaction_box_active;}

	engine::bounding_box get_interaction_hitbox() { return interaction_hitbox; }


	void start_crouch() { crouched = true; }
	void end_crouch() { crouched = false; }
	bool is_crouch() { return crouched; }

	void start_sprint() {sprinting = true;}
	void end_sprint() {sprinting = false;}
	void reduce_health(float damage) {
		health -= damage;
		if (health <= 0) {
			health = 0;
			alive = false;
		}
	}

	float get_health() { return health; }
	bool is_alive() { return alive; }


	engine::ref<engine::game_object>& get_player_object() { return player_object; }
	std::vector<engine::ref<projectile>>& get_active_projectiles() { return active_projectiles; }


private:
	glm::vec3 player_pos;
	engine::bounding_box player_hitbox;
	engine::bounding_box interaction_hitbox; 
	engine::perspective_camera& player_camera;

	bool interaction_box_active = false; 

	bool alive = true;
	float health = 100;
	float speed;

	engine::ref<engine::game_object>	m_claw{};


	//Movement
	float movement_speed = 2.5f;
	bool sprinting = false;
	float sprint_speed = 1.f;
	//Sprint stepping variable
	float sprint_step = .06f;
	float max_sprint_speed = 3.5f;
	float min_sprint_speed = 1.f;
	float sprint_mulitplayer = 1.f;

	//Crouch
	bool crouched = false;
	float crouch_step = 2.0f;
	float standing_height = 1.5f;
	float crouching_height = 1.0f;

	std::vector<engine::ref<engine::game_object>>& m_game_objects;

	
	engine::game_object_properties bl_props{};
	std::vector<engine::ref<projectile>> active_projectiles{};

	engine::ref<engine::game_object> flintlock_object{};
	engine::ref<engine::game_object> player_object{};
	engine::ref<engine::material>  m_tricube_material{};

	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;

	float time_since_last_shot = 0.f;

};
