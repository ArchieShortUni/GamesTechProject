#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"

player::player(engine::perspective_camera& camera):player_camera(camera) {
	
}


void player::initialise() {
	
	player_hitbox.set_box(1.f, 1.f, 1.f, player_camera.position());
	interaction_hitbox.set_box(.5f, .5f, .5f, player_camera.front_vector() * glm::vec3(2.f));

	engine::ref<engine::model> claw_model = engine::model::create("assets/models/static/claw.obj");
	engine::game_object_properties claw_props;

	engine::ref<engine::cuboid> cube = engine::cuboid::create(glm::vec3(.5f), false);

	engine::game_object_properties cube_props;
	cube_props.position = glm::vec3(1.f);
	cube_props.meshes = { cube->mesh() };
	
	m_claw = engine::game_object::create(cube_props);
	
}

void player::on_render(engine::ref<engine::shader> shader){
	//player_hitbox.on_render(2.5f, 0.f, 0.f, shader);
	if (interaction_box_active) { interaction_hitbox.on_render(2.5f, 0.f, 0.f, shader); };
	engine::ref < engine::material> m_tricube_material = engine::material::create(32.0f,
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		0.3f);

	m_tricube_material->submit(shader);
	glm::vec3 pos = player_camera.position();
	glm::vec3 forward = player_camera.front_vector();
	glm::vec3 right = player_camera.right_vector();
	float theta = engine::PI / 2.0f - acos(forward.y);
	float phi = atan2(forward.x, forward.z);
	glm::vec3 p = pos + 0.5f * forward + 0.2f * right;
	glm::mat4 transform(1.0f);
	glm::translate(transform, glm::vec3(1.f));

	glm::rotate(transform, phi, glm::vec3(0.f, 1.f, 0.f));
	glm::rotate(transform, -theta, glm::vec3(1.f, 0.f, 0.f));
	///glm::scale(transform, m_claw->scale());
	engine::renderer::submit(shader, transform, m_claw);
}

void player::on_update(const engine::timestep& time_step){


	glm::vec3 interaction_pos = player_camera.position() + (player_camera.front_vector()* glm::vec3(1.5f));
	interaction_pos.y -= .25f;
	player_pos = glm::vec3(player_camera.position());
	glm::vec3 hitboxPos = glm::vec3(player_camera.position());
	
	hitboxPos.y -= .5f;
	player_hitbox.on_update(hitboxPos);
	interaction_hitbox.on_update(interaction_pos);

	

}

void player::update_player_data(glm::vec3 new_position){
	player_camera.set_position(new_position);
}
