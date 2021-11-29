#include "pch.h"
#include "game_manager.h"
#include "quad.h"

game_manager::game_manager(engine::perspective_camera& camera) {

	//Hard code beacon data
	glm::vec3 beacon1_pos = glm::vec3(3.f,.5f,10.f);
	glm::vec3 beacon2_pos = glm::vec3(14.f,.5f,-7.f);
	glm::vec3 beacon3_pos = glm::vec3(-10.f,.5f,5.f);

	

	engine::ref<beacon> m_beacon1 = beacon::create(bcolour1,beacon1_pos,3,25.0f,2,2.f);
	engine::ref<beacon> m_beacon2 = beacon::create(bcolour2, beacon2_pos, 3, 25.0f, 2,1.5f);
	engine::ref<beacon> m_beacon3 = beacon::create(bcolour3, beacon3_pos, 3, 25.0f, 2,1.f);

	level_beacons.push_back(m_beacon1);
	level_beacons.push_back(m_beacon2);
	level_beacons.push_back(m_beacon3);

	m_text_manager = engine::text_manager::create();

	level_complete_percent = level_beacons.size() * 100;

	level_bar = progress_bar::create(.03f,.8f, glm::vec2(.0f, 0.8f),  10.f, "assets/textures/bar_back.bmp" , "assets/textures/bar_back.bmp", "assets/textures/menu.bmp");

	beacon1_bar = progress_bar::create(.02f, .3f, glm::vec2(-1.25f, -0.4f), 10.f, "assets/textures/bar_back.bmp", "assets/textures/bar_front.bmp", "assets/textures/energy_yellow.bmp");
	beacon2_bar = progress_bar::create(.02f, .3f, glm::vec2(-1.25f, -0.5f), 10.f, "assets/textures/bar_back.bmp", "assets/textures/bar_front.bmp", "assets/textures/energy_yellow.bmp");
	beacon3_bar = progress_bar::create(.02f, .3f, glm::vec2(-1.25f, -0.62f), 10.f, "assets/textures/bar_back.bmp", "assets/textures/bar_front.bmp", "assets/textures/energy_yellow.bmp");

	//Player class info
	player = player::create(camera);
	player->initialise();

}


void game_manager::on_update(const engine::timestep& time_step) {
	//Player control stuff
	player->on_update(time_step);

	engine::bounding_box player_hitbox = player->get_hitbox();
	


	int beacons_add_percent = 0;
	for (int i = 0; i < level_beacons.size(); i++) {
		level_beacons.at(i)->on_update(time_step);
		beacons_add_percent += level_beacons.at(i)->get_percent();

		std::vector<engine::ref<beacon_switch>> switches = level_beacons.at(i)->get_switches();

		for (int j = 0; j < switches.size(); j++) {
			if (switches.at(j)->get_hitbox().collision(player_hitbox)) {
				player->update_player_data(last_player_pos);
			}

			if (player->get_interaction()) {
				if (switches.at(j)->get_hitbox().collision(player->get_interaction_hitbox())) {
					switches.at(j)->swap_state();
					player->interact_false();
				}
			}

		}


	}



	last_player_pos = player->get_player_position();
	//update the total percentage

	current_level_percent = (100 / level_complete_percent)*beacons_add_percent;

	level_bar->update_bar((int)current_level_percent);
	beacon1_bar->update_bar((int)level_beacons.at(0)->get_percent());
	beacon2_bar->update_bar((int)level_beacons.at(1)->get_percent());
	beacon3_bar->update_bar((int)level_beacons.at(2)->get_percent());






}

void game_manager::on_render2d(engine::ref<engine::shader> shader) {
	//Beacon Data 
	level_bar->on_render2d(shader);
	beacon1_bar->on_render2d(shader);
	beacon2_bar->on_render2d(shader);
	beacon3_bar->on_render2d(shader);

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	std::string percent1 = std::to_string(level_beacons.at(0)->get_percent());
	std::string percent2 = std::to_string(level_beacons.at(1)->get_percent());
	std::string percent3 = std::to_string(level_beacons.at(2)->get_percent());
	std::string percent4 = std::to_string((int)current_level_percent);

	percent1.erase(percent1.find_last_not_of('0') + 1, std::string::npos);
	percent2.erase(percent2.find_last_not_of('0') + 1, std::string::npos);
	percent3.erase(percent3.find_last_not_of('0') + 1, std::string::npos);

	percent1.pop_back();
	percent2.pop_back();
	percent3.pop_back();

	std::string b_p1 = "Beacon 1 Percentage: "+percent1;
	std::string b_p2 = "Beacon 2 Percentage: " + percent2;
	std::string b_p3 = "Beacon 3 Percentage: " + percent3;
	std::string overall_percentage = "Overall Percentage: " + percent4;

	m_text_manager->render_text(text_shader, b_p1, 20.f, (float)engine::application::window().height() - 500.f, 0.3f, glm::vec4(bcolour1,1.f));
	m_text_manager->render_text(text_shader, b_p2, 20.f, (float)engine::application::window().height() - 545.f, 0.3f, glm::vec4(bcolour2, 1.f));
	m_text_manager->render_text(text_shader, b_p3, 20.f, (float)engine::application::window().height() - 590.f, 0.3f, glm::vec4(bcolour3, 1.f));
	//m_text_manager->render_text(text_shader, overall_percentage, 10.f, (float)engine::application::window().height() - 590.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));






}
void game_manager::on_render3d(engine::ref<engine::shader> shader) {
	for (int i = 0; i < level_beacons.size(); i++) {
		level_beacons.at(i)->on_render(shader); 
	}

	
	player->on_render(shader);
}

engine::ref<game_manager> game_manager::create(engine::perspective_camera& camera)
{
	return std::make_shared<game_manager>(camera);
}
