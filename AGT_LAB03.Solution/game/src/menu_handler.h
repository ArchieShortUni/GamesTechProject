#pragma once
#include <engine.h>
#include "engine/entities/bounding_box_bullet.h"
#include "menu_enemy.h"
class quad;

class menu_handler {
public:
	menu_handler(float width, float height, engine::ref<engine::game_object>& bg);
	~menu_handler();

	void on_update(const engine::timestep& time_step); 
	void on_render2d(engine::ref<engine::shader> shader);
	void on_render3d(engine::ref<engine::shader> shader);
	void swap();

	static engine::ref<menu_handler> create(float width, float height, engine::ref<engine::game_object>& bg);

	bool on_menu() { return menu_active; }

	

private:
	bool menu_active = true;
	engine::ref<engine::bullet_manager> m_physics_manager{};

	std::vector<engine::ref<menu_enemy>> m_enemies{};

	engine::ref<engine::game_object>	m_back_terrain{};
	engine::ref<engine::game_object> m_letter{};
	std::vector<engine::ref<engine::game_object>> m_menuitems{};
	std::vector<engine::bounding_box> m_boxes{};

	engine::SpotLight					m_spotLight;
	uint32_t							num_spot_lights = 1;

	engine::PointLight m_pointLight;
	uint32_t num_point_lights = 1;
	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;


};
