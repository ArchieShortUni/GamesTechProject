#pragma once

#include <engine.h>

#include"engine/entities/bounding_box_bullet.h"
class beacon_switch {
public:
	beacon_switch( glm::vec3 colour) {
		base_colour = colour;

		std::vector<glm::vec3> tricube_vertices;
		//Rectange Vertices
		tricube_vertices.push_back(glm::vec3(-5.f, 8.f, 5.f)); //0
		tricube_vertices.push_back(glm::vec3(5.f, 8.f, 5.f)); //1
		tricube_vertices.push_back(glm::vec3(5.f, 8.f, -5.f)); //2
		tricube_vertices.push_back(glm::vec3(-5.f, 8.f, -5.f)); //3
		tricube_vertices.push_back(glm::vec3(-5.f, 10.f, 5.f)); //4
		tricube_vertices.push_back(glm::vec3(5.f, 10.f, 5.f)); //5
		tricube_vertices.push_back(glm::vec3(5.f, 10.f, -5.f)); //6
		tricube_vertices.push_back(glm::vec3(-5.f, 10.f, -5.f)); //7
		//Triangle Tip
		tricube_vertices.push_back(glm::vec3(0.f, 23.f, 0.f)); //8
		//Base vertices
		tricube_vertices.push_back(glm::vec3(-1.f, 8.f, 1.f)); //9
		tricube_vertices.push_back(glm::vec3(1.f, 8.f, 1.f)); //10
		tricube_vertices.push_back(glm::vec3(1.f, 8.f, -1.f)); //11
		tricube_vertices.push_back(glm::vec3(-1.f, 8.f, -1.f)); //12
		tricube_vertices.push_back(glm::vec3(-1.f, 0.f, 1.f)); //13
		tricube_vertices.push_back(glm::vec3(1.f, 0.f, 1.f)); //14
		tricube_vertices.push_back(glm::vec3(1.f, 0.f, -1.f)); //15
		tricube_vertices.push_back(glm::vec3(-1.f, 0.f, -1.f)); //16

		engine::ref<engine::tricube> tricube_shape = engine::tricube::create(tricube_vertices);
	

		sw_props.meshes = { tricube_shape->mesh() };
		sw_props.scale /= 15;
		sw_props.position = glm::vec3(0.f, 1.f, 0.f);
		sw_props.bounding_shape = glm::vec3(1.f,1.2f, 1.f);
		sw_props.type = 0;
		sw_props.mass = 10000;

		switch_box.set_box(sw_props.bounding_shape.x,
			sw_props.bounding_shape.y,
			sw_props.bounding_shape.z,
			glm::vec3(sw_props.position.x, sw_props.position.y - .3f , sw_props.position.x));

		switch_obj = engine::game_object::create(sw_props);

		switch_obj->set_angular_factor_lock(true);
		//Creates black starting material
		m_switch_material = engine::material::create(1.0f, glm::vec3(0.0f, 0.f, .0f),
			glm::vec3(0.0f, 0.f, .0f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
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

		//switch_box.on_render(2.5f, 0.f, 0.f, shader);
	}

	void on_update() {
		switch_box.on_update(switch_obj->position(), switch_obj->rotation_amount(), switch_obj->rotation_axis());
	}

	void set_position(glm::vec3 position) { switch_obj->set_position(position);
	switch_box.on_update(glm::vec3(position.x, position.y, position.z), switch_obj->rotation_amount(), switch_obj->rotation_axis());
	}

	glm::vec3 get_position() { return switch_obj->position(); }

	void swap_state() {
		//Swaps material to black or whatever the full colour should be depending on if the switch on is true or false 
		if (is_on) { is_on = false; m_switch_material->set_ambient(glm::vec3(0.0f, 0.f, .0f)); m_switch_material->set_diffuse(glm::vec3(0.0f, 0.f, .0f));
		}
		else {
			is_on = true; m_switch_material->set_ambient(glm::vec3(base_colour));  m_switch_material->set_diffuse(glm::vec3(base_colour));
		}
	}
	bool get_state() { return is_on; }

	engine::bounding_box get_hitbox() { return switch_box; }

	engine::ref<engine::game_object>& get_switch_object() { return switch_obj; }
private:
	bool is_on = false;
	glm::vec3 position;
	glm::vec3 base_colour;
	engine::game_object_properties sw_props{};
	engine::ref<engine::game_object> switch_obj; 
	engine::ref<engine::material> m_switch_material;

	engine::bounding_box	switch_box;

};

class beacon {
public:
	beacon(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius,int switches_to_activate,float b_speed,int light_number);
	~beacon();

	static engine::ref<beacon> create(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius, int switches_to_activate, float b_speed, int light_number);
	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& time_step);

	void swap_state();
	float get_height() { return current_beacon_height; }
	float get_percent() { return floor((double)percentage + 0.5); }

	engine::ref<engine::game_object>& get_object() { return m_beacon; }
	void new_switches_pos(float range,float min_range);

	int get_active_switches() {
		int active = 0;
		for (int i = 0; i < switches.size(); i++) { if (switches.at(i)->get_state()) { active++; } }
		return active;
	}

	std::vector<engine::ref<beacon_switch>>& get_switches() { return switches; }


	void beam_speed_boost() {
		beam_speed += .5f;
	}

private:
	bool beacon_active = true;
	float scale_factor = 3.f;

	int sw_to_activate;
	//How quickly the beam percentage increases 
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
	engine::bounding_box	beacon_box;

	//Lighting
	engine::PointLight m_pointLight;
	uint32_t num_point_lights = 3;
	engine::ref<engine::material>		m_lightsource_material{};
	int light_int;

	//Temporarily using cubes for the switches, swap to primitive or mesh object later
	std::vector<engine::ref<beacon_switch>> switches;

};
