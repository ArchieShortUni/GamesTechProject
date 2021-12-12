#include "pch.h"
#include "enemy_ranged.h"

enemy_ranged::enemy_ranged(glm::vec3 pos, float h, engine::ref<player> targ, bool hasShotgun) :target(targ),shotgun(hasShotgun){
	std::string path;
	if(hasShotgun){
		path = "assets/models/static/crab_shotgun.obj";
	}
	else {
		path = "assets/models/static/crab_gun.obj";
	}
	engine::ref<engine::model> crab_model = engine::model::create(path);
	health = h;
	enemy_props.meshes = crab_model->meshes();
	enemy_props.textures = crab_model->textures();
	float e_scale = 3.f / glm::max(crab_model->size().x, glm::max(crab_model->size().y, crab_model->size().z));
	e_scale *= .35f;
	enemy_props.position = pos;
	enemy_props.bounding_shape = glm::vec3(1.2f * e_scale, .4f * e_scale, 1.2f * e_scale);
	enemy_props.type = 0;
	enemy_props.mass = 5;

	
	enemy_props.scale = glm::vec3(e_scale);

	m_enemy = engine::game_object::create(enemy_props);

	engine::ref<engine::model> bullet_model = engine::model::create("assets/models/static/bullet.obj");

	bl_props.meshes = bullet_model->meshes();
	bl_props.textures = bullet_model->textures();
	float bl_scale = 3.f / glm::max(bullet_model->size().x, glm::max(bullet_model->size().y, bullet_model->size().z));
	bl_scale /= 30.f;
	bl_props.scale = glm::vec3(bl_scale);
	bl_props.rotation_amount = AI_DEG_TO_RAD(-100);
	bl_props.mass = .1f;

	hitbox.set_box(enemy_props.bounding_shape.x, enemy_props.bounding_shape.y+.4f , enemy_props.bounding_shape.z, m_enemy->position());

//Behaviour Tree Setup
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &sequence[0],&patrol_action });
	sequence[0].addChildren({ &targetInSight,&selector[1] });
	selector[1].addChildren({ &sequence[1],&approach_action });
	sequence[1].addChildren({ &targetInRange,&shoot_action });

}

void enemy_ranged::on_render(engine::ref<engine::shader> shader) {
	if (m_state != game_enums::state::attemptingUnstick) {
		//hitbox.on_render(2.5f, .0f, .0f, shader);
	}
	glm::mat4 enemy_transform(1.f);

	glm::vec3 v2 = glm::normalize(target->get_player_position() - m_enemy->position());
	glm::vec3 side_vector = glm::cross(glm::vec3(0.f, 1.f, 0.f), v2);
	glm::vec3 rotation_vector = -glm::cross(side_vector, v2);
	float theta3 = atan2(v2.x, v2.z);

	enemy_transform = glm::translate(enemy_transform, m_enemy->position());

	if (m_state == game_enums::state::approach || m_state == game_enums::state::shoot) {
		enemy_transform = glm::rotate(enemy_transform, theta3 + AI_DEG_TO_RAD(-90), glm::vec3(0.f, 1.f, 0.f));

	}
	else {
		enemy_transform = glm::rotate(enemy_transform, m_enemy->rotation_amount(), m_enemy->rotation_axis());

	}


	enemy_transform = glm::scale(enemy_transform, m_enemy->scale());

	engine::renderer::submit(shader, enemy_transform, m_enemy);

	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_render(shader);
	}

}

void enemy_ranged::on_update(const engine::timestep& time_step) {
	time_since_last_shot += 1.f * time_step;
	time_alive += 1.f * time_step;
	glm::vec3 hitboxpos = m_enemy->position();
	hitboxpos.y -= .25f;
	hitbox.on_update(hitboxpos, m_enemy->rotation_amount(), m_enemy->rotation_axis());

	if(time_alive > 2.f){

		//Defining decision variables for behaviour Tree
		if (glm::distance(target->get_player_position(), m_enemy->position()) < 20.f) {
			player_in_sight = true;
			if (glm::distance(target->get_player_position(), m_enemy->position()) < 15.f) {
				player_in_range = true;
			}
			else { player_in_range = false; }
		}
		else {
			player_in_sight = false;
		}

		behaviorTree.run();
		m_state = next_state;




		if (time_alive > last_second) {
			if ((m_state == game_enums::state::patrol || m_state == game_enums::state::approach) && (glm::distance(previous_position, m_enemy->position()) < .2f)) {
				m_last_state = m_state;
				m_state = game_enums::state::stuck;
				stuck_point = m_enemy->position();
			}
			last_second += 2;
			previous_position = m_enemy->position();

		}

		


		//AI Things
		if (m_state == game_enums::state::patrol) {
			if (reached_patrol_point) {
				generate_patrol_point(30.f, .5f);
				reached_patrol_point = false;
			}
			patrol(time_step);

			if (glm::distance(m_enemy->position(), patrol_point) < 2.f) {
				reached_patrol_point = true;
			}
		}

		if (m_state == game_enums::state::stuck) {
			if (m_last_state == game_enums::state::patrol) {
				temp_point = patrol_point;
				generate_patrol_point(15.f, 0.4f);
				m_state = game_enums::state::attemptingUnstick;
			}


		}

		if (m_state == game_enums::state::attemptingUnstick) {
			time_since_stuck += 1.f * time_step;
			if (m_last_state == game_enums::state::patrol) {
				patrol(time_step);
				if (glm::distance(stuck_point, m_enemy->position()) > 4.f) {
					m_state = m_last_state;
					patrol_point = temp_point;
				}
				if (time_since_stuck > 4.f) {
					generate_patrol_point(15.f, 0.4f);
					time_since_stuck = 0;
				}
			}
		}

		if (m_state == game_enums::state::shoot) {
			if (time_since_last_shot > .3f) {
				shoot();
				time_since_last_shot = 0.f;
			}
		}

		if (m_state == game_enums::state::approach) {
			approach(time_step);
		}

		for (int i = 0; i < active_projectiles.size(); i++) {
			active_projectiles.at(i)->on_update(time_step);

			if (active_projectiles.at(i)->get_hitbox().collision(target->get_hitbox())) {
				active_projectiles.erase(active_projectiles.begin() + i);
				target->reduce_health(2);
			}

			else if (active_projectiles.at(i)->get_time_alive() > 3.f) {
				active_projectiles.erase(active_projectiles.begin() + i);
			}
		}


		
	}
}

void enemy_ranged::generate_patrol_point(float radius, float min_range) {
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

		if (z <min_range && z>-min_range) {
			z = (((float)rand()) / RAND_MAX - .5f);
		}
		else { z_range_correct = true; }

	}
	x *= radius;
	z *= radius;

	patrol_point = glm::vec3(x, m_enemy->position().y, z);

}

//void enemy_ranged::shoot() {
//	if (shotgun) {
//		for(int i=0; i<6;i++){
//
//		std::random_device rd;     // only used once to initialise (seed) engine
//		std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
//		std::uniform_int_distribution<int> uni(0, 15); // guaranteed unbiased
//
//		float angle = uni(rng);
//		angle = AI_RAD_TO_DEG(angle);
//
//		engine::ref<projectile> bullet = projectile::create(bl_props, 1);
//		active_projectiles.push_back(bullet);
//		glm::vec3 p = m_enemy->position();
//		glm::vec3 c = target->get_player_position();
//		glm::vec3 v = c - p;
//		v.y -= 1.f;
//		glm::vec3 firing_pos = m_enemy->position();
//		firing_pos += (m_enemy->forward() / 4.f) - (m_enemy->right() / 1.5f);
//		//firing_pos.y += 1.2f;
//
//		bullet->fire(firing_pos, glm::normalize(v), 5.f);
//		}
//	}
//	else  {
//		engine::ref<projectile> bullet = projectile::create(bl_props, 1);
//		active_projectiles.push_back(bullet);
//		glm::vec3 p = m_enemy->position();
//		glm::vec3 c = target->get_player_position();
//		glm::vec3 v = c - p;
//		v.y -= 1.f;
//		glm::vec3 firing_pos = m_enemy->position();
//		firing_pos += (m_enemy->forward() / 4.f) - (m_enemy->right() / 1.5f);
//		//firing_pos.y += 1.2f;
//
//		bullet->fire(firing_pos, glm::normalize(v), 5.f);
//	}
//
//}

void enemy_ranged::shoot() {

	if(shotgun){
		for (int i = 0; i < 5; i++) {
			engine::ref<projectile> bullet = projectile::create(bl_props, 1);
			active_projectiles.push_back(bullet);

			glm::vec3 firing_pos = m_enemy->position();
			firing_pos += (m_enemy->forward() / 4.f) - (m_enemy->right() / 1.5f);

			glm::vec3 c = target->get_player_position();
			c.y += .5;
			glm::vec3 v = c - firing_pos;
			v.y -= 1.f;

			v.x += (((float)rand()) / RAND_MAX - .5f);
			v.y += (((float)rand()) / RAND_MAX - .5f);
			v.z += (((float)rand()) / RAND_MAX - .5f); 
			//Randomise V 
			bullet->fire(firing_pos, glm::normalize(v), 5.f);
		}
	}
	else{
		engine::ref<projectile> bullet = projectile::create(bl_props, 1);
		active_projectiles.push_back(bullet);

		glm::vec3 firing_pos = m_enemy->position();
		firing_pos += (m_enemy->forward() / 4.f) - (m_enemy->right() / 1.5f);

		glm::vec3 c = target->get_player_position();
		c.y += .5;
		glm::vec3 v = c - firing_pos;
		v.y -= 1.f;

		bullet->fire(firing_pos, glm::normalize(v), 5.f);
	}
}

void enemy_ranged::patrol(const engine::timestep& time_step) {
	m_enemy->set_forward(glm::normalize(patrol_point - m_enemy->position()));
	m_enemy->set_velocity(m_enemy->forward() * movement_speed);
	//m_enemy->set_position(m_enemy->position() + m_enemy->forward() * movement_speed * (float)time_step);
	//m_enemy->set_rotation_amount(atan2(m_enemy->forward().x, m_enemy->forward().z));
}

void enemy_ranged::approach(const engine::timestep& time_step) {
	glm::vec3 future_pos = target->get_future_position(3.5f);
	future_pos.y = m_enemy->position().y;
	m_enemy->set_forward(glm::normalize(future_pos - m_enemy->position()));
	m_enemy->set_velocity(m_enemy->forward() * movement_speed * glm::vec3(2.f));

	

}
