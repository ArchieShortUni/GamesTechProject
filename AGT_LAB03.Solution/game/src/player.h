#pragma once
#include <engine.h>
#include "engine/entities/bounding_box.h"

class player {
public:
	player(engine::perspective_camera& camera);
	~player(){}

	static engine::ref<player> create(engine::perspective_camera& camera) {
		return std::make_shared<player>(camera);
	}

	//void initialise(engine::perspective_camera& camera);

	void initialise();
	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);
	void update_player_data(glm::vec3 new_position);

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
private:
	glm::vec3 player_pos;
	engine::bounding_box player_hitbox;
	engine::bounding_box interaction_hitbox; 
	engine::perspective_camera& player_camera;

	bool interaction_box_active = false; 

	float health = 100;
	float speed;

	engine::ref<engine::game_object>	m_claw{};


	//Test
	engine::ref<engine::game_object> test_cube{};

};
