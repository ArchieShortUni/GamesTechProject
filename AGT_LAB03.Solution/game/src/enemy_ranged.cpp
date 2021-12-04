#include "pch.h"
#include "enemy_ranged.h"

enemy_ranged::enemy_ranged(glm::vec3 pos, float h, engine::ref<player> targ) :target(targ){
	engine::ref<engine::model> crab_model = engine::model::create("assets/models/static/crab.obj");
	health = h;
	enemy_props.meshes = crab_model->meshes();
	enemy_props.textures = crab_model->textures();
	float e_scale = 3.f / glm::max(crab_model->size().x, glm::max(crab_model->size().y, crab_model->size().z));
	e_scale *= .4f;
	enemy_props.position = pos;
	enemy_props.bounding_shape = glm::vec3(2.9f * e_scale, .4f * e_scale, 3.2f * e_scale);
	enemy_props.type = 0;

	
	enemy_props.scale = glm::vec3(e_scale);

	m_enemy = engine::game_object::create(enemy_props);

	hitbox.set_box(enemy_props.bounding_shape.x, enemy_props.bounding_shape.y+.8f , enemy_props.bounding_shape.z, m_enemy->position());
}

void enemy_ranged::on_render(engine::ref<engine::shader> shader) {
	if (m_state != state::attemptingUnstick) {
		hitbox.on_render(2.5f, .0f, .0f, shader);
	}
	glm::mat4 enemy_transform(1.f);
	enemy_transform = glm::translate(enemy_transform, m_enemy->position());
	enemy_transform = glm::rotate(enemy_transform, m_enemy->rotation_amount(), m_enemy->rotation_axis());
	enemy_transform = glm::scale(enemy_transform, m_enemy->scale());

	engine::renderer::submit(shader, enemy_transform, m_enemy);
}

void enemy_ranged::on_update(const engine::timestep& time_step) {
	//Test code to make it work
	if (time_alive < 2.f) {
		time_alive += 1.f * time_step;
	}
	else {
		m_state = state::patrol;
		time_alive += 1.f * time_step;
	}

	if (time_alive > last_second) {
		if ((m_state == state::patrol || m_state == state::approach) && (glm::distance(previous_position, m_enemy->position()) < .2f)) {
			m_last_state = m_state;
			m_state = state::stuck;
			stuck_point = m_enemy->position();
		}
		last_second +=2;
		previous_position = m_enemy->position();
		
	}

	time_since_last_shot += 1.f * time_step;
	glm::vec3 hitboxpos = m_enemy->position();
	hitboxpos.z -= .2f;
	hitboxpos.x -= .1f;
	hitbox.on_update(hitboxpos, m_enemy->rotation_amount(), m_enemy->rotation_axis());


	//AI Things
	if (m_state == state::patrol) {
		if (reached_patrol_point) {
			generate_patrol_point(30.f, .5f);
			reached_patrol_point = false;
		}
		patrol(time_step);

		if (glm::distance(m_enemy->position(), patrol_point) < 2.f) {
			reached_patrol_point = true;
		}
	}

	if (m_state == state::stuck) {
		if (m_last_state == state::patrol) {
			temp_point = patrol_point;
			generate_patrol_point(15.f, 0.4f);
			m_state = state::attemptingUnstick;
		}
		
	}
	if (m_state == state::attemptingUnstick) {
		time_since_stuck += 1.f * time_step;
		if (m_last_state == state::patrol) {
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

void enemy_ranged::patrol(const engine::timestep& time_step) {


	m_enemy->set_forward(glm::normalize(patrol_point - m_enemy->position()));
	m_enemy->set_velocity(m_enemy->forward() * movement_speed);
	//m_enemy->set_position(m_enemy->position() + m_enemy->forward() * movement_speed * (float)time_step);
	//m_enemy->set_rotation_amount(atan2(m_enemy->forward().x, m_enemy->forward().z));
}
