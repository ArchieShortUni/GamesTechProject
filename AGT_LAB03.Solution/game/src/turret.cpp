#include "pch.h"
#include "turret.h"

turret::turret(glm::vec3 pos, float , engine::ref<player> targ):target(targ) {
	pos.y -= 1.f;
	std::vector<glm::vec3> turret_vertices;
	
	/// <summary>
	/// //////////
	/// </summary>

	

	//Left Triangle Base
	turret_vertices.push_back(glm::vec3(-3.f, .0f, 1.5f));	//0
	turret_vertices.push_back(glm::vec3(-1.5f, .0f, 1.5f));	//1
	turret_vertices.push_back(glm::vec3(-1.5f, .0f, -1.5f));	//2
	turret_vertices.push_back(glm::vec3(-3.f, .0f, -1.5f));//3
	turret_vertices.push_back(glm::vec3(-1.5f, 6.f, 1.5f));	//4
	turret_vertices.push_back(glm::vec3(-1.5f, 6.f, -1.5f));	//5

	//Right Triange Base
	turret_vertices.push_back(glm::vec3(1.5f, .0f, 1.5f));	//6
	turret_vertices.push_back(glm::vec3(3.f, .0f, 1.5f));	//7 
	turret_vertices.push_back(glm::vec3(3.f, .0f, -1.5f));//8
	turret_vertices.push_back(glm::vec3(1.5f, .0f, -1.5f));	//9
	turret_vertices.push_back(glm::vec3(1.5f, 6.f, 1.5f));	//10
	turret_vertices.push_back(glm::vec3(1.5f, 6.f, -1.5f));	//11

	//Turret Part
	turret_vertices.push_back(glm::vec3(-1.5, 4.f, 3.f));	//12
	turret_vertices.push_back(glm::vec3(1.5f, 4.f, 3.f));	//13
	turret_vertices.push_back(glm::vec3(1.5f, 4.f, -5.f));	//14
	turret_vertices.push_back(glm::vec3(-1.5f, 4.f, -5.f));	//15

	turret_vertices.push_back(glm::vec3(-1.5, 6.f, 3.f));	//16
	turret_vertices.push_back(glm::vec3(1.5f, 6.f, 3.f));	//17
	turret_vertices.push_back(glm::vec3(1.5f, 6.f, -5.f));	//18
	turret_vertices.push_back(glm::vec3(-1.5f,6.f, -5.f));	//19



	engine::ref<engine::turret_shape> turret_shape = engine::turret_shape::create(turret_vertices);
	engine::game_object_properties turret_props;

	


	turret_props.position = glm::vec3(pos.x,pos.y,pos.z);
	turret_props.meshes = { turret_shape->mesh() };
	turret_props.scale /= 8;
	turret_props.rotation_amount = AI_DEG_TO_RAD(180);

	
	m_turret = engine::game_object::create(turret_props);

	m_disabled_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_active_material = engine::material::create(1.0f, glm::vec3(.10f, 1.f, 0.07f),
		glm::vec3(.10f, 1.f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);



	engine::ref<engine::cuboid> base_cube = engine::cuboid::create(glm::vec3(.25f, .3f, .25f), false, false);
	engine::game_object_properties base_props;
	base_props.meshes = { base_cube->mesh() };
	base_props.rotation_amount = AI_DEG_TO_RAD(270);
	base_props.scale = glm::vec3(1.f);
	base_props.position = glm::vec3(pos.x,pos.y-.2f,pos.z);
	base_props.bounding_shape = glm::vec3(1.f, 1.f, 1.f);
	base_props.type = 0;
	base_props.mass = 100000;

	base_hitbox.set_box(.8f,1.3f,.8f,
		base_props.position);

	m_base = engine::game_object::create(base_props);

	engine::ref<engine::model> bullet_model = engine::model::create("assets/models/static/bullet.obj");

	bl_props.meshes = bullet_model->meshes();
	bl_props.textures = bullet_model->textures();
	float bl_scale = 3.f / glm::max(bullet_model->size().x, glm::max(bullet_model->size().y, bullet_model->size().z));
	bl_scale /= 30.f;
	bl_props.scale = glm::vec3(bl_scale);
	bl_props.rotation_amount = AI_DEG_TO_RAD(-100);
	bl_props.mass = .1f;
}

void turret::on_render(engine::ref<engine::shader> shader) {
	base_hitbox.on_render(2.5f, 0.f, 0.f, shader);
	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_render(shader);
	}

	if (is_active) {
		m_active_material->submit(shader);
		glm::mat4 turret_transform(1.f);
		glm::vec3 p = m_turret->position();
		glm::vec3 c = target->get_player_position();
		glm::vec3 v = c - p;

		float theta = atan2(v.x, v.z);
		theta += AI_DEG_TO_RAD(180);
		last_angle = theta;
		turret_transform = glm::translate(turret_transform, p);
		turret_transform = glm::rotate(turret_transform, theta, glm::vec3(0.f, 1.f, 0.f));
		

		turret_transform = glm::scale(turret_transform, m_turret->scale());
		
		engine::renderer::submit(shader, turret_transform, m_turret);
		engine::renderer::submit(shader, m_base);
		

	}
	else {
		m_disabled_material->submit(shader);
		glm::mat4 turret_transform(1.f);
		turret_transform = glm::translate(turret_transform, m_turret->position());
		turret_transform = glm::rotate(turret_transform, last_angle, glm::vec3(0.f, 1.f, 0.f));
		turret_transform = glm::scale(turret_transform, m_turret->scale());
		engine::renderer::submit(shader, turret_transform,m_turret);
		engine::renderer::submit(shader, m_base);
	}
}

void turret::on_update(const engine::timestep& time_step) {
	glm::vec3 hitboxpos = m_base->position();
	hitboxpos.y -= .3f;
	base_hitbox.on_update(hitboxpos, m_base->rotation_amount(), m_base->rotation_axis());
	time_since_last_shot += 1 * time_step;

	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_update(time_step);

		if (active_projectiles.at(i)->get_hitbox().collision(target->get_hitbox())) {
			active_projectiles.erase(active_projectiles.begin() + i);
			target->reduce_health(2);
		}

		else if(active_projectiles.at(i)->get_time_alive() > 3.f) {
			active_projectiles.erase(active_projectiles.begin() + i);
			//active_projectiles.at(i)->~projectile();
		}
	}

	


	glm::vec3 distance = target->get_player_position() - m_turret->position();
	float distance_num= sqrt((distance.x * distance.x) + (distance.z * distance.z ));
	if (distance_num > 10.f) {
		is_active = false;
	}
	else {
		is_active = true;
		if (time_since_last_shot > .3f) {
			engine::ref<projectile> bullet = projectile::create(bl_props, 1);
			active_projectiles.push_back(bullet);
			glm::vec3 p = m_turret->position();
			glm::vec3 c = target->get_player_position();
			glm::vec3 v = c - p;
			v.y -= 1.f;

			glm::vec3 firing_pos = m_turret->position();
			firing_pos.y += .6f;
			

			bullet->fire(firing_pos, glm::normalize(v), 5.f);
			time_since_last_shot = 0.f;
		}
	}
}


engine::ref<turret> turret::create(glm::vec3 pos, float h, engine::ref<player> targ)
{
	return std::make_shared<turret>(pos, h, targ);
}
