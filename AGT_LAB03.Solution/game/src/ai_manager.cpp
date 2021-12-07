#include "pch.h"
#include "ai_manager.h"

ai_manager::ai_manager(std::vector<engine::ref<engine::game_object>>& game_objects, engine::ref<player>& p, std::vector<engine::ref<beacon_switch>>& switches):m_game_objects(game_objects),target(p), level_switches(switches){
	m_explosion = explosion::create("assets/textures/Explosion.tga", 12, 1, 12);
}


void ai_manager::add_enemy(game_enums::enemies enemyType, glm::vec3 pos) {
		if (enemyType == game_enums::enemies::ranged) {
		engine::ref<enemy_ranged> en = enemy_ranged::create(pos, 100, target,false);
		m_game_objects.push_back(en->get_object());
		active_ranged_enemies.push_back(en);
	}
		else if (enemyType == game_enums::enemies::engineer) {
			engine::ref<enemy_engineer> en = enemy_engineer::create(pos, 100, target, level_switches,active_turrets);
			m_game_objects.push_back(en->get_object());
			active_engineer_enemies.push_back(en);
		}
		else if (enemyType == game_enums::enemies::rangedShotgun) {
			engine::ref<enemy_ranged> en = enemy_ranged::create(pos, 100, target, true);
			m_game_objects.push_back(en->get_object());
			active_ranged_enemies.push_back(en);
		}
		else if (enemyType == game_enums::enemies::melee) {
			engine::ref<enemy_melee> en = enemy_melee::create(pos, 100, target);
			m_game_objects.push_back(en->get_object());
			active_melee_enemies.push_back(en);
		}
}


void ai_manager::on_render(engine::ref<engine::shader> shader) {
	m_explosion->on_render(target->get_camera(),shader);
	//Ranged enemies
	for (int i = 0; i < active_ranged_enemies.size(); i++) {
		active_ranged_enemies.at(i)->on_render(shader);
	}

	for (int i = 0; i < active_engineer_enemies.size(); i++) {
		active_engineer_enemies.at(i)->on_render(shader);
	}

	for (int i = 0; i < active_melee_enemies.size(); i++) {
		active_melee_enemies.at(i)->on_render(shader);
	}

	for (int i = 0; i < active_turrets.size(); i++) {
		active_turrets.at(i)->on_render(shader);
	}
}

void ai_manager::on_update(const engine::timestep& time_step) {
	m_explosion->on_update(time_step);

	for (int i = 0; i < active_ranged_enemies.size(); i++) {
		active_ranged_enemies.at(i)->on_update(time_step);
		if (active_ranged_enemies.at(i)->get_health() == 0) {
			m_explosion->activate(active_ranged_enemies.at(i)->get_pos(), 2.f, 2.f);
			active_ranged_enemies.erase(active_ranged_enemies.begin() + i);

		}
	}

	for (int i = 0; i < active_engineer_enemies.size(); i++) {
		active_engineer_enemies.at(i)->on_update(time_step);
		if (active_engineer_enemies.at(i)->get_health() == 0) {
			m_explosion->activate(active_engineer_enemies.at(i)->get_pos(), 2.f, 2.f);
			active_engineer_enemies.erase(active_engineer_enemies.begin() + i);
		}
	}

	for (int i = 0; i < active_melee_enemies.size(); i++) {
		active_melee_enemies.at(i)->on_update(time_step);
		if (active_melee_enemies.at(i)->get_health() == 0) {
			m_explosion->activate(active_melee_enemies.at(i)->get_pos(), 2.f, 2.f);
			active_melee_enemies.erase(active_melee_enemies.begin() + i);
		}
	}

	for (int i = 0; i < active_turrets.size(); i++) {
		active_turrets.at(i)->on_update(time_step);
		if (active_turrets.at(i)->get_health() == 0) {
			m_explosion->activate(active_turrets.at(i)->get_pos(), 2.f, 2.f);
			active_turrets.erase(active_turrets.begin() + i);
		}
	}

	std::vector<engine::ref<projectile>>& player_projectiles = target->get_active_projectiles();
	for (int i = 0; i < player_projectiles.size(); i++) {
		//Turret Collision
		bool hit = false;
		if (!hit) {
			for (int t = 0; t < active_ranged_enemies.size(); t++) {
				if ((player_projectiles.at(i)->get_hitbox().collision(active_ranged_enemies.at(t)->get_hit_box())) && !hit) {
					active_ranged_enemies.at(t)->take_damage(50);
					hit = true;
				}
			}
		}
		if (!hit) {
			for (int t = 0; t < active_engineer_enemies.size(); t++) {
				if ((player_projectiles.at(i)->get_hitbox().collision(active_engineer_enemies.at(t)->get_hit_box())) && !hit) {
					active_engineer_enemies.at(t)->take_damage(50);
					hit = true;
				}
			}
		}

		if (!hit) {
			for (int t = 0; t < active_turrets.size(); t++) {
				if ((player_projectiles.at(i)->get_hitbox().collision(active_turrets.at(t)->get_hit_box())) && !hit) {
					active_turrets.at(t)->take_damage(50);
					hit = true;
				}
			}
		}

		if (!hit) {
			for (int t = 0; t < active_melee_enemies.size(); t++) {
				if ((player_projectiles.at(i)->get_hitbox().collision(active_melee_enemies.at(t)->get_hit_box())) && !hit) {
					active_melee_enemies.at(t)->take_damage(50);
					hit = true;
				}
			}
		}
		if (hit) {
			player_projectiles.erase(player_projectiles.begin() + i);
		}
	}
}


