#include "pch.h"
#include "beacon.h"


beacon::beacon(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius, int switches_to_activate, float b_speed, int light_number) {
	beacon_position = position;
	beacon_position.y += 1.f;
	sw_to_activate = switches_to_activate;
	beam_speed = b_speed;
	light_int = light_number;

	engine::ref<engine::model> b_model = engine::model::create("assets/models/static/beacon.obj");
	m_cube = engine::cuboid::create(glm::vec3(.08f*scale_factor, .3f * scale_factor, .08f * scale_factor), false,false);
	
	b_props.meshes = b_model->meshes();
	b_props.textures = b_model->textures();
	float b_scale = 3.f / glm::max(b_model->size().x, glm::max(b_model->size().y, b_model->size().z));
	b_scale *=scale_factor;
	b_props.position = beacon_position;
	b_props.bounding_shape = glm::vec3(.6f, .04f, .6f);
	b_props.rotation_amount = AI_DEG_TO_RAD(270);
	b_props.scale = glm::vec3(b_scale);
	b_props.mass = 100.f;


	beacon_box.set_box(b_props.bounding_shape.x,
		b_props.bounding_shape.y,
		b_props.bounding_shape.z,
		glm::vec3(b_props.position.x, b_props.position.y - .3f, b_props.position.x));

	m_beacon = engine::game_object::create(b_props);

	m_material = engine::material::create(0.0f, colour, colour, colour, 0.4f);

	m_pointLight.Color = colour;
	m_pointLight.AmbientIntensity = 0.25f;
	m_pointLight.DiffuseIntensity = 0.6f;
	m_pointLight.Position = glm::vec3(beacon_position.x, beacon_position.y + 2.f, beacon_position.z);

	m_lightsource_material = engine::material::create(1.0f, m_pointLight.Color,
		m_pointLight.Color, m_pointLight.Color, 1.0f);

	//Generate switch positions,
	for (int i = 0; i < switch_num; i++) {
		engine::ref<beacon_switch> sw = beacon_switch::create(colour);
		switches.push_back(sw);
	}
	
	new_switches_pos(switch_radius,0.5f); 
}


beacon::~beacon() {}


void beacon::new_switches_pos(float radius,float min_range) {
	for (int i = 0; i < switches.size(); i++) {
		bool correct_range = false;
		float x = (((float)rand()) / RAND_MAX - .5f);
		float z = (((float)rand()) / RAND_MAX - .5f);

		bool x_range_correct = false;
		bool z_range_correct = false;
		while (!x_range_correct && !z_range_correct) {
			if (x <min_range && x>-min_range) {
				x = (((float)rand()) / RAND_MAX - .5f);
			}
			else { x_range_correct = true; }

			if(z <min_range && z>-min_range){
				z = (((float)rand()) / RAND_MAX - .5f);
			}
			else { z_range_correct = true; }

		}
		x *= radius;
		z *= radius; 

		
		switches.at(i)->set_position(glm::vec3(m_beacon->position().x+x, m_beacon->position().y+3.f, m_beacon->position().z+z));
		//switches.at(i)->set_position(glm::vec3(m_beacon->position().x , m_beacon->position().y, m_beacon->position().z ));

	}
}

void beacon::on_render(engine::ref<engine::shader> shader) {
	if(light_int == 0){
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("gNumPointLights", (int)num_point_lights);
}
	m_pointLight.submit(shader, light_int);

	beacon_box.on_render(2.5f, .0f, .0f, shader);

	glm::mat4 beacon_transform(1.f);
	beacon_transform = glm::translate(beacon_transform, m_beacon->position());
	beacon_transform = glm::rotate(beacon_transform, m_beacon->rotation_amount(), m_beacon->rotation_axis());
	beacon_transform = glm::scale(beacon_transform, m_beacon->scale());
	engine::renderer::submit(shader, beacon_transform, m_beacon);

	for (int i = 0; i < switches.size(); i++) {
		switches.at(i)->on_render(shader);
	}
	//m_material->submit(shader);

	m_lightsource_material->submit(shader);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("lighting_on", false);

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, beacon_position);
	transform = glm::scale(transform, glm::vec3(1.f, current_beacon_height, 1.f));
	transform = glm::rotate(transform, beam_rotation, glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(shader, m_cube->mesh(), transform);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("lighting_on", true);
}

void beacon::on_update(const engine::timestep& time_step) {
	beacon_box.on_update(m_beacon->position(), m_beacon->rotation_amount(), m_beacon->rotation_axis());
	if (get_active_switches() < sw_to_activate && beacon_active) {
		swap_state();
	}
	else if (get_active_switches() >= sw_to_activate && !beacon_active) {
		swap_state(); 
	}

	if (beacon_active) {

		if (!(current_beacon_height >= max_beacon_height)) {
			current_beacon_height += (float)time_step * beam_speed;
		}
		else { current_beacon_height = max_beacon_height; }

		

	}
	else {
		if (!(current_beacon_height <= min_beacon_height)) {
			current_beacon_height -= (float)time_step * (beam_speed/2);
		}
		else { current_beacon_height = min_beacon_height; }

	}

	beam_rotation += (float)time_step;
	percentage = (100 / (max_beacon_height)) * current_beacon_height;

	for (int i = 0; i < switches.size(); i++) {
		switches.at(i)->on_update();
	}

}

void beacon::swap_state() {
	if (beacon_active) { beacon_active = false; }
	else { beacon_active = true; }
}

engine::ref<beacon> beacon::create(glm::vec3 colour,glm::vec3 position, int switch_num, float switch_radius, int switches_to_activate, float b_speed, int light_number)
{
	return std::make_shared<beacon>(colour,position, switch_num, switch_radius, switches_to_activate, b_speed, light_number);
}
