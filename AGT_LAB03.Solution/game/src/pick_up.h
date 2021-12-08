#pragma once
#include <engine.h>
#include "player.h"
#include"engine/entities/bounding_box_bullet.h"

class pickup {
public:
	pickup(glm::vec3 pos, std::vector<engine::ref<beacon>>& beacons,engine::ref<player>& p, engine::ref<engine::audio_manager>& audio):level_beacons(beacons),player(p),m_audio_manager(audio) {
		pickup_type = (rand() % 2) != 0;
		std::string path = "assets/textures/";

		if (pickup_type) {
			path += "circuit.bmp";
		}
		else {
			path += "circuit2.bmp";
		}
		

		std::vector<glm::vec3> pickup_vertices;

		pickup_vertices.push_back(glm::vec3(0.f,5.f,0.f));		//0
		pickup_vertices.push_back(glm::vec3(0.f, 0.f, -2.f));	//1
		pickup_vertices.push_back(glm::vec3(2.f, 0.f, -1.f));	//2	
		pickup_vertices.push_back(glm::vec3(2.f, 0.f, 1.f));	//3
		pickup_vertices.push_back(glm::vec3(0.f, 0.f, 2.f));	//4
		pickup_vertices.push_back(glm::vec3(-2.f, 0.f, 1.f));	//5
		pickup_vertices.push_back(glm::vec3(-2.f, 0.f, -1.f));	//6
		pickup_vertices.push_back(glm::vec3(0.f, -5.f, 0.f));	//7

		//engine::ref<engine::model> shell_model = engine::model::create("assets/models/static/shell.obj");
		std::vector<engine::ref<engine::texture_2d>> pickup_textures =
		{ engine::texture_2d::create(path, false) };

		

		engine::ref<engine::pickup_shape> pickup_shape = engine::pickup_shape::create(pickup_vertices);

		engine::game_object_properties p_props;
		p_props.meshes = { pickup_shape->mesh() };
		p_props.textures = pickup_textures;
		//p_props.meshes = shell_model->meshes();
		//p_props.textures = shell_model->textures();
		p_props.scale /= 8;
		p_props.position = pos;
		p_props.bounding_shape = glm::vec3(1.f, 1.f, 1.f);
		p_props.type = 0;

		m_pickup = engine::game_object::create(p_props);

		hitbox.set_box(.5f, 1.3f, .5f, glm::vec3(m_pickup->position().x, m_pickup->position().y, m_pickup->position().z));
	}
	~pickup(){}

	static engine::ref<pickup> create(glm::vec3 pos ,std::vector<engine::ref<beacon>>& beacons, engine::ref<player>& p, engine::ref<engine::audio_manager>& audio) {
		return std::make_shared<pickup>(pos,beacons,p,audio);
	}

	void on_render(engine::ref<engine::shader> shader) {
		glm::mat4 switch_transform(1.0f);
		switch_transform = glm::translate(switch_transform, m_pickup->position());
		switch_transform = glm::rotate(switch_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		switch_transform = glm::scale(switch_transform, m_pickup->scale());
		engine::renderer::submit(shader, switch_transform, m_pickup);

		hitbox.on_render(2.5f, 0.f, 0.f, shader);
	}

	void on_update(const engine::timestep& time_step) {
		glm::vec3 hitboxpos = m_pickup->position();
		hitboxpos.y -= .55f;
		hitbox.on_update(hitboxpos, m_pickup->rotation_amount(), m_pickup->rotation_axis());
	}

	void on_pickup() {
		m_audio_manager->play_spatialised_sound("pick_up", player->get_camera().position(), m_pickup->position());
		if (pickup_type) {
			std::random_device rd;     // only used once to initialise (seed) engine
			std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
			std::uniform_int_distribution<int> uni(0, level_beacons.size() - 1); // guaranteed unbiased
			int tempIterator = uni(rng);
			level_beacons.at(tempIterator)->beam_speed_boost();
		}
		else {
			player->increase_health(20);
		}
	}

	engine::bounding_box get_hitbox() { return hitbox; } 
private:
	engine::bounding_box hitbox;
	engine::ref<engine::game_object> m_pickup;
	bool pickup_type; 
	std::vector<engine::ref<beacon>>& level_beacons;
	engine::ref<player>& player;
	engine::ref<engine::audio_manager>& m_audio_manager;

};
