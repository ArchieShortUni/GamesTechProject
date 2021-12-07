#include "pch.h"
#include "enemy_engineer.h"

enemy_engineer::enemy_engineer(glm::vec3 pos, float h, engine::ref<player> targ, std::vector<engine::ref<beacon_switch>>& switches, std::vector<engine::ref<turret>>& all_turrets) :target(targ), level_switches(switches),active_turrets(all_turrets) {
	engine::ref<engine::model> crab_model = engine::model::create("assets/models/static/crab_engineer.obj");
	health = h;
	enemy_props.meshes = crab_model->meshes();
	enemy_props.textures = crab_model->textures();
	float e_scale = 3.f / glm::max(crab_model->size().x, glm::max(crab_model->size().y, crab_model->size().z));
	e_scale *= .35f;
	enemy_props.position = pos;
	enemy_props.bounding_shape = glm::vec3(.4f * e_scale, .4f * e_scale, .4f * e_scale);
	enemy_props.type = 0;


	enemy_props.scale = glm::vec3(e_scale);

	m_enemy = engine::game_object::create(enemy_props);

	hitbox.set_box(enemy_props.bounding_shape.x+.7f, enemy_props.bounding_shape.y + .4f, enemy_props.bounding_shape.z+.7f, m_enemy->position());

	//Behaviour Tree Setup

	/*behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &sequence[0],&patrol_action });
	sequence[0].addChildren({ &targetInSight,&selector[1] });
	selector[1].addChildren({ &sequence[1],&approach_switch_action });
	sequence[1].addChildren({ &targetInRange,&shoot_action });*/

	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &sequence[0],&selector[5] });
	sequence[0].addChildren({ &targetOutSight,&selector[1] });
	selector[1].addChildren({&sequence[1],&selector[3]}); //Change Later
	//Turning Switch off decisions

	sequence[1].addChildren({ &anySwitchesOn,&selector[2] });
	selector[2].addChildren({&sequence[2],&approach_switch_action});
	sequence[2].addChildren({ &atSwitch,&turn_off_Switch_action });

	//Placing protective Turret Decision

	selector[3].addChildren({ &sequence[3],&patrol_action });
	sequence[3].addChildren({ &turretNotPlaced,&selector[4] });
	selector[4].addChildren({ &sequence[4],&approach_switch_action });
	sequence[4].addChildren({ &atSwitch,&placing_turret_action });

	//Place Turret and Flee decisions

	selector[5].addChildren({ &sequence[5],&flee_action });
	sequence[5].addChildren({ &offensiveTurretReady,&placing_offensive_turret_action });
}

void enemy_engineer::on_render(engine::ref<engine::shader> shader) {
	if (m_state != game_enums::state::attemptingUnstick) {
		hitbox.on_render(2.5f, .0f, .0f, shader);
	}
	glm::mat4 enemy_transform(1.f);
	enemy_transform = glm::translate(enemy_transform, m_enemy->position());
	enemy_transform = glm::rotate(enemy_transform, m_enemy->rotation_amount(), m_enemy->rotation_axis());
	enemy_transform = glm::scale(enemy_transform, m_enemy->scale());

	engine::renderer::submit(shader, enemy_transform, m_enemy);
	for (int i = 0; i < active_projectiles.size(); i++) {
		active_projectiles.at(i)->on_render(shader);
	}

}

void enemy_engineer::on_update(const engine::timestep& time_step) {
	time_since_last_shot += 1.f * time_step;
	time_alive += 1.f * time_step;
	time_since_last_turret += 1.f * time_step;
	glm::vec3 hitboxpos = m_enemy->position();
	//hitboxpos.z -= .2f;
	hitboxpos.y -= .25f;
	hitbox.on_update(hitboxpos, m_enemy->rotation_amount(), m_enemy->rotation_axis());



	if (time_alive > 2.f) {

		//Defining decision variables for behaviour Tree
		if (glm::distance(target->get_player_position(), m_enemy->position()) < 10.f) {
			player_out_sight = false;

		}
		else {
			player_out_sight = true;
		}

		switches_on = any_switches_on();

		at_switch = next_to_on_switch();

		if (time_since_last_turret > 5.f) {
			offensive_turret_ready = true;
		}
		else
		{
			offensive_turret_ready = false;
		}
	


		behaviorTree.run(); 
		if (m_state != game_enums::state::attemptingUnstick) {
			m_state = next_state;
		}




		if (time_alive > last_second) {
			if ((m_state == game_enums::state::patrol || (m_state == game_enums::state::approach && !at_switch)) && (glm::distance(previous_position, m_enemy->position()) < .1f)) {
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
			else if (m_last_state == game_enums::state::approach) {
				temp_point = closest_switch_pos;
				generate_patrol_point(10.f, 0.4f); 
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
			else if (m_last_state == game_enums::state::approach) {
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

	

		if (m_state == game_enums::state::approach) {
			if (switches_on) {
				closest_switch(true);
			}
			else {
				closest_switch(false);
			}

			approach(closest_switch_pos);
		}

		if (m_state == game_enums::state::turnOffSwitch) {
			turn_off_closest_switch();
			glm::vec3 p = m_enemy->position();
			p += m_enemy->right();
			place_turret(p);
		}

		if (m_state == game_enums::state::placingTurret) {
			glm::vec3 p = m_enemy->position();
			p += m_enemy->right();
			place_turret(p);
		}

		if (m_state == game_enums::state::flee) {
			flee();
		}

		if (m_state == game_enums::state::placingOffensiveTurret) {
			time_since_last_turret = 0.f;
			glm::vec3 p = m_enemy->position();
			p += m_enemy->right();
			place_turret(p);
		}
		//Projectile Handling

		for (int i = 0; i < active_projectiles.size(); i++) {
			active_projectiles.at(i)->on_update(time_step);

			if (active_projectiles.at(i)->get_hitbox().collision(target->get_hitbox())) {
				active_projectiles.erase(active_projectiles.begin() + i);
				target->reduce_health(2);
			}

			else if (active_projectiles.at(i)->get_time_alive() > 3.f) {
				active_projectiles.erase(active_projectiles.begin() + i);
				//active_projectiles.at(i)->~projectile();
			}
		}
	}
}

void enemy_engineer::generate_patrol_point(float radius, float min_range) {
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

void enemy_engineer::shoot() {

	engine::ref<projectile> bullet = projectile::create(bl_props, 1);
	active_projectiles.push_back(bullet);
	glm::vec3 p = m_enemy->position();
	glm::vec3 c = target->get_player_position();
	glm::vec3 v = c - p;
	v.y -= 1.f;

	glm::vec3 firing_pos = m_enemy->position();
	firing_pos.y += .6f;


	bullet->fire(firing_pos, glm::normalize(v), 5.f);


}

void enemy_engineer::patrol(const engine::timestep& time_step) {
	m_enemy->set_forward(glm::normalize(patrol_point - m_enemy->position()));
	m_enemy->set_velocity(m_enemy->forward() * movement_speed);
	//m_enemy->set_position(m_enemy->position() + m_enemy->forward() * movement_speed * (float)time_step);
	//m_enemy->set_rotation_amount(atan2(m_enemy->forward().x, m_enemy->forward().z));
}

void enemy_engineer::approach(glm::vec3 targetPos) {
	if(glm::distance(targetPos,m_enemy->position()) >2.f){
	m_enemy->set_forward(glm::normalize(targetPos - m_enemy->position()));
	m_enemy->set_velocity(m_enemy->forward() * movement_speed * glm::vec3(2.f));

	}
	else { m_enemy->set_velocity(glm::vec3(0.f));  }
}

bool enemy_engineer::any_switches_on(){
	for (int i = 0; i < level_switches.size(); i++) {
		if (level_switches.at(i)->get_state()) {
			return true;
		}
	}
	return false;
}

void enemy_engineer::closest_switch(bool on) {
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, level_switches.size()-1); // guaranteed unbiased

	float distance = 1000.f;
	int tempIterator = uni(rng);

	for (int i = 0; i < level_switches.size(); i++) {
		if (level_switches.at(i)->get_state() == on) {
			glm::vec3 tempVec = level_switches.at(i)->get_position();
			if (glm::distance(tempVec, m_enemy->position()) < distance) {
				distance = glm::distance(tempVec, m_enemy->position());
				tempIterator = i;
			}
		}
	}
	closest_switch_int = tempIterator;
	glm::vec3 newpos = level_switches.at(tempIterator)->get_position();
	newpos.y = m_enemy->position().y;
	closest_switch_pos = newpos;
}


void enemy_engineer::turn_off_closest_switch() {
	if (level_switches.at(closest_switch_int)->get_state() == true) {
		level_switches.at(closest_switch_int)->swap_state();
	}
	
}

bool enemy_engineer::next_to_on_switch() {
	closest_switch(true);
	if (glm::distance(m_enemy->position(), closest_switch_pos) > 2.f) {
		return false;
	}
	return true;
}

void enemy_engineer::place_turret(glm::vec3 pos) {
	pos.y = 2.1f;
	engine::ref<turret> t = turret::create(pos, 100, target);
	active_turrets.emplace_back(t);
	turret_not_placed = false;
	std::cout << "PLACING TURRET";
}

void enemy_engineer::flee() {
	glm::vec3 dir = glm::normalize(target->get_player_position() - m_enemy->position());
	dir.x *= -1;
	dir.z *= -1;
	dir.y = 0;
	m_enemy->set_forward(dir);
	m_enemy->set_velocity(m_enemy->forward() * movement_speed * glm::vec3(2.f));
}
