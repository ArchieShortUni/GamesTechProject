#pragma once
#include <engine.h>

class quad;

class menu_handler {
public:
	menu_handler(float width, float height);
	~menu_handler();


	void on_render2d(engine::ref<engine::shader> shader);
	void on_render3d(engine::ref<engine::shader> shader);
	void swap();

	static engine::ref<menu_handler> create(float width, float height);

	bool on_menu() { return menu_active; }

	

private:
	bool menu_active = true;

	engine::ref<engine::game_object>	m_crab{};
	engine::ref<engine::game_object> m_letter{};
	std::vector<engine::ref<engine::game_object>> m_menuitems{};

	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;


};
