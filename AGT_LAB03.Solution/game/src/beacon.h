#pragma once

#include <engine.h>

#include"engine/entities/bounding_box_bullet.h"
class beacon_switch {
public:
	beacon_switch( glm::vec3 colour) {
		base_colour = colour;
		engine::ref<engine::cuboid> sw_cube = engine::cuboid::create(glm::vec3(.4f, .4f, .4f), false);
		sw_props.meshes = { sw_cube->mesh() };
		sw_props.rotation_amount = AI_DEG_TO_RAD(270);
		sw_props.scale = glm::vec3(1.f);
		sw_props.position = glm::vec3(0.f, -2.f, 0.f);
		sw_props.bounding_shape = glm::vec3(1.f, 1.f, 1.f);
		sw_props.type = 0;
		sw_props.mass = 100000;

		switch_box.set_box(sw_props.bounding_shape.x * sw_props.scale.x,
			sw_props.bounding_shape.y * sw_props.scale.x,
			sw_props.bounding_shape.z * sw_props.scale.x,
			sw_props.position);

		switch_obj = engine::game_object::create(sw_props);

		switch_obj->set_angular_factor_lock(true);

		m_switch_material = engine::material::create(1.0f, colour,
			colour, glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	}

	~beacon_switch() {}

	static engine::ref<beacon_switch> create(glm::vec3 colour) {
		return std::make_shared<beacon_switch>(colour);
	}

	void on_render(engine::ref<engine::shader> shader) {
		m_switch_material->submit(shader);
		glm::mat4 switch_transform(1.0f);
		switch_transform = glm::translate(switch_transform, switch_obj->position());
		switch_transform = glm::rotate(switch_transform, switch_obj->rotation_amount(), switch_obj->rotation_axis());
		switch_transform = glm::scale(switch_transform, switch_obj->scale());
		engine::renderer::submit(shader, switch_transform, switch_obj);

		switch_box.on_render(2.5f, 0.f, 0.f, shader);
	}

	void set_position(glm::vec3 position) { switch_obj->set_position(position);
	switch_box.on_update(glm::vec3(position.x, position.y - .5f, position.z),switch_obj->rotation_amount(), switch_obj->rotation_axis());
	}

	void swap_state() {
		if (is_on) { is_on = false; m_switch_material->set_ambient(glm::vec3(1.0f, 0.64706f, .0f)); }
		else { is_on = true; m_switch_material->set_ambient(glm::vec3(base_colour));
		}
	}
	bool get_state() { return is_on; }

	engine::bounding_box get_hitbox() { return switch_box; }

	engine::ref<engine::game_object>& get_switch_object() { return switch_obj; }
private:
	bool is_on = true;
	glm::vec3 position;
	glm::vec3 base_colour;
	engine::game_object_properties sw_props{};
	engine::ref<engine::game_object> switch_obj; 
	engine::ref<engine::material> m_switch_material;

	engine::bounding_box	switch_box;

};

class beacon {
public:
	beacon(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius,int switches_to_activate,float b_speed);
	~beacon();

	static engine::ref<beacon> create(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius, int switches_to_activate, float b_speed);
	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);

	void swap_state();
	float get_height() { return current_beacon_height; }
	float get_percent() { return floor((double)percentage + 0.5); }

	void new_switches_pos(float range,float min_range);

	int get_active_switches() {
		int active = 0;
		for (int i = 0; i < switches.size(); i++) { if (switches.at(i)->get_state()) { active++; } }
		return active;
	}

	std::vector<engine::ref<beacon_switch>>& get_switches() { return switches; }



private:
	bool beacon_active = true;
	float scale_factor = 3.f;

	int sw_to_activate;
	float beam_speed = 0;
	float max_beacon_height = 170.f;
	float min_beacon_height = 9.f;
	float percentage = 0.f;
	glm::vec3 beacon_position; 

	float current_beacon_height = 9.f;
	float beam_rotation = 0.f;

	engine::game_object_properties b_props{};

	engine::ref<engine::material> m_material;
	engine::ref<engine::cuboid> m_cube;
	engine::ref<engine::game_object> m_beacon{};

	//Temporarily using cubes for the switches, swap to primitive or mesh object later
	std::vector<engine::ref<beacon_switch>> switches;

};
