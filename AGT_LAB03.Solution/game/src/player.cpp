#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"


player::player(engine::perspective_camera& camera, std::vector<engine::ref<engine::game_object>>& game_objs, engine::ref<engine::audio_manager>& audio):player_camera(camera),m_game_objects(game_objs),m_audio_manager(audio) {
	
}


void player::initialise(float width, float height) {
	engine::ref<engine::cuboid> cube = engine::cuboid::create(glm::vec3(.1f,.1f,.1f), false,false);
	engine::game_object_properties player_props;
	player_props.meshes = { cube->mesh() };
	player_props.scale = glm::vec3(1.f);
	player_props.position = glm::vec3(0.f, 1.f, 1.f);
	player_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	player_props.type = 0;
	player_props.bounding_shape = glm::vec3(.2f, 1.f, .2f);
	player_props.mass = .0001f;
	player_object = engine::game_object::create(player_props);
	player_object->set_angular_factor_lock(true);

	player_hitbox.set_box(.2f, .7f, .2f, player_object->position());
	interaction_hitbox.set_box(.5f, .5f, .5f, player_camera.front_vector() * glm::vec3(2.f));

	engine::ref<engine::model> flintlock_model = engine::model::create("assets/models/static/flintlockPixel.obj");
	engine::game_object_properties fl_props{};
	fl_props.meshes = flintlock_model->meshes();
	fl_props.textures = flintlock_model->textures();
	float fl_scale = 3.f / glm::max(flintlock_model->size().x, glm::max(flintlock_model->size().y, flintlock_model->size().z));
	fl_scale /= 3.f;
	fl_props.scale = glm::vec3(fl_scale);
	fl_props.rotation_amount = AI_DEG_TO_RAD(-100);

	flintlock_object = engine::game_object::create(fl_props);
	/// 
	/// Projectile Creation 
	///

	engine::ref<engine::model> bullet_model = engine::model::create("assets/models/static/bullet.obj");

	bl_props.meshes = bullet_model->meshes();
	bl_props.textures = bullet_model->textures();
	float bl_scale = 3.f / glm::max(bullet_model->size().x, glm::max(bullet_model->size().y, bullet_model->size().z));
	bl_scale /=40.f;
	bl_props.scale = glm::vec3(bl_scale);
	bl_props.rotation_amount = AI_DEG_TO_RAD(-100);
	bl_props.mass = .1f;

	

	m_tricube_material = engine::material::create(32.0f,
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		1.0f);


	//CROSSHAIR
	m_texture = engine::texture_2d::create("assets/textures/crosshair.bmp", true);
	m_quad = quad::create(glm::vec2(width, height));

	//Lighting
	m_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.AmbientIntensity = 0.55f;
	m_spotLight.DiffuseIntensity = 0.8f;
	m_spotLight.Position = glm::vec3(-5.0f, 2.0f, 0.0f);
	m_spotLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	m_spotLight.Cutoff = 0.0001f;
	m_spotLight.Attenuation.Constant = 1.0f;
	m_spotLight.Attenuation.Linear = 0.1f;
	m_spotLight.Attenuation.Exp = 0.01f;
}

void player::on_render2d(engine::ref<engine::shader> shader) {

	glm::mat4 transform(1.0f);
//	transform = glm::translate(transform, glm::vec3(.1f, -0.025f, 0.1f));
	transform = glm::translate(transform, glm::vec3(.0f, .0f, -0.1f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);

}
void player::on_render(engine::ref<engine::shader> shader){
	

	player_hitbox.on_render(2.5f, 0.f, 0.f, shader);
	if (interaction_box_active) { interaction_hitbox.on_render(2.5f, 0.f, 0.f, shader); };

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_spotLight.submit(shader, 0);

	m_tricube_material->submit(shader);
	engine::renderer::submit(shader, player_object);

	glm::vec3 pos = player_camera.position();
	//pos.y += .5f;
	glm::vec3 forward = player_camera.front_vector();
	glm::vec3 right = player_camera.right_vector();

	float theta = engine::PI / 2.0f - acos(forward.y);
	float phi = atan2(forward.x, forward.z);

	glm::vec3 p = pos + 0.4f * forward + 0.32f * right;
	p.y -= .2f;

	glm::mat4 flintlock_transform(1.f);
	//flintlock_transform = glm::translate(flintlock_transform, player_camera.position() + (player_camera.front_vector() * glm::vec3(1.5f)));
	flintlock_transform = glm::translate(flintlock_transform, p);
	flintlock_transform = glm::rotate(flintlock_transform, phi+flintlock_object->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
	flintlock_transform = glm::rotate(flintlock_transform, theta, glm::vec3(0.f, 0.f, 1.f));
	//flintlock_transform = glm::rotate(flintlock_transform, flintlock_object->rotation_amount(), flintlock_object->rotation_axis());
	flintlock_transform = glm::scale(flintlock_transform, flintlock_object->scale());
	engine::renderer::submit(shader, flintlock_transform, flintlock_object);

	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_render(shader);
	}

	
	
}

void player::on_update(const engine::timestep& time_step){
	
	m_spotLight.Direction = glm::normalize(player_camera.front_vector());
	glm::vec3 spotPos = player_camera.position();
	glm::vec3 forward = player_camera.front_vector();
	glm::vec3 right = player_camera.right_vector();
	float theta = engine::PI / 2.0f - acos(forward.y);
	float phi = atan2(forward.x, forward.z);

	glm::vec3 p = spotPos - 0.2f * forward  - 0.1f * right;
	p.y -= .2f;
	m_spotLight.Position = p;



	time_since_last_shot += 1.f * time_step;
	time_since_last_jump += 1.f * time_step;

	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_update(time_step);
		if (active_projectiles.at(i)->get_time_alive() > 3.f) {
			active_projectiles.erase(active_projectiles.begin() + i);
			//active_projectiles.at(i)->~projectile();
		}
	}

	

	glm::vec3 interaction_pos = player_camera.position() + (player_camera.front_vector()* glm::vec3(1.5f));
	interaction_pos.y -= .25f;
	player_pos = glm::vec3(player_camera.position());
	glm::vec3 hitboxPos = glm::vec3(player_object->position());
	
	//hitboxPos.y -= .5f;
	player_hitbox.on_update(hitboxPos,player_object->rotation_amount(),player_object->rotation_axis());
	interaction_hitbox.on_update(interaction_pos, player_object->rotation_amount(), player_object->rotation_axis());

	

	/// 
	/// MOVEMENT
	/// 
	if(up_speed >= 0){
		up_speed += gravity * time_step;
	}

	
	//player_object->set_up(player_object->up() + (gravity * time_step));

	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left 
	{
		player_object->set_velocity(-(movement_speed * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z))));

		glm::vec3 new_pos = player_object->position() - (movement_speed * time_step * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z)));
		//player_object->set_position(new_pos);
		ld = lastdirection::left;
	}
	//	move(e_direction::left, time_step);
	else if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right 
		//move(e_direction::right, time_step);
	{
		player_object->set_velocity(movement_speed * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z)));
		glm::vec3 new_pos = player_object->position() + (movement_speed * time_step * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z)));
		//player_object->set_position(new_pos);
		ld = lastdirection::right;

	}

	 if (engine::input::key_pressed(engine::key_codes::KEY_S)) // down 
		//move(e_direction::backward, time_step);
	{
		player_object->set_velocity(-(movement_speed *glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z))));
		glm::vec3 new_pos = player_object->position() - (movement_speed * time_step * glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z)));
		//player_object->set_position(new_pos);
		ld = lastdirection::back;

	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_W)) // up 
		//move(e_direction::forward, time_step);
	{
		player_object->set_velocity(movement_speed * glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z)));
		glm::vec3 new_pos = player_object->position() + (movement_speed * time_step * glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z)));
		//player_object->set_position(new_pos);
		ld = lastdirection::foward;

	}



	/*
	else if (engine::input::key_pressed(engine::key_codes::KEY_SPACE)) {
		//player_object->set_velocity(movement_speed*100.f* glm::normalize(glm::vec3(0.f, player_camera.up_vector().y,0.f)));
		if (time_since_last_jump > 1.5f) {
			up_speed = .8f;
			time_since_last_jump = 0;
		}
	
	}

	player_object->set_velocity(player_object->velocity() + (player_object->up() * up_speed));

	*/


	
	///
	/// DELETE LATER
	///
	/*
	
	if (engine::input::key_pressed(engine::key_codes::KEY_T)) {
		t = turret::create(player_camera.position(), 100,);
		active_turrets.push_back(t);
	}*/

	//SHOOTING
	if (engine::input::mouse_button_pressed(0)) {
		if (time_since_last_shot > .5f) {

			glm::vec3 pos = player_camera.position();
			glm::vec3 forward = player_camera.front_vector();
			glm::vec3 right = player_camera.right_vector();
			glm::vec3 p = pos + 0.9f * forward + 0.4f * right;
			p.y -= .1f;

			m_audio_manager->play_spatialised_sound("gun_shot", player_camera.position(), p);
			m_audio_manager->volume("gun_shot", 0.002f);

			engine::ref<projectile> bullet = projectile::create(bl_props, 0);
			active_projectiles.push_back(bullet);
		//	m_game_objects.push_back(bullet->get_projectile_object());
			
			bullet->fire(p, player_camera.front_vector(), 10.f);
			time_since_last_shot = 0.f;
		}
	}


	

	/*
	if (player_object->position().y > standing_height) { player_object->set_position(glm::vec3(current_pos.x, standing_height, current_pos.z)); }
	else if (player_object->position().y < crouching_height) { player_object->set_position(glm::vec3(current_pos.x, crouching_height, current_pos.z)); }

	current_pos = player_object->position();
	if (!crouched) {
		if (player_object->position().y < standing_height) {
			player_object->set_position(glm::vec3(current_pos.x, current_pos.y + (crouch_step * time_step), current_pos.z));
		}
	}
	else {
		if (player_object->position().y > crouching_height) {
			player_object->set_position(glm::vec3(current_pos.x, current_pos.y - (crouch_step * time_step), current_pos.z));
		}
	}

	*/
	if (!sprinting) {
		if (sprint_speed > min_sprint_speed) {
			sprint_speed -= (sprint_step * (sprint_mulitplayer * sprint_mulitplayer)) * time_step;
			sprint_mulitplayer += .4f;
		}
		else { sprint_mulitplayer = 1.f; }

	}
	else {
		if (sprint_speed < max_sprint_speed) {
			sprint_speed += (sprint_step * (sprint_mulitplayer * sprint_mulitplayer)) * time_step;
			sprint_mulitplayer += .2f;
		}
		else { sprint_mulitplayer = 1.f; }
	}

}


void player::update_camera() {
	glm::vec3 current = player_object->position();
	glm::vec3 new_pos = glm::vec3(current.x, current.y + .5f, current.z);
	player_camera.set_position(new_pos);
}

glm::vec3 player::get_future_position(float seconds_in_future) {
	glm::vec3 position = player_object->position();
	if (ld == lastdirection::foward) {
		position += (movement_speed * glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z))) * seconds_in_future; 
	}
	else if (ld == lastdirection::back) {
		position += -(movement_speed * glm::normalize(glm::vec3(player_camera.front_vector().x, 0, player_camera.front_vector().z))) * seconds_in_future; 
	}
	else if (ld == lastdirection::left) {
		position += -(movement_speed * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z)))*seconds_in_future;
	}
	else if (ld == lastdirection::right) {
		position += (movement_speed * sprint_speed * glm::normalize(glm::vec3(player_camera.right_vector().x, 0, player_camera.right_vector().z)))* seconds_in_future;
	}

	return position;

}
