#pragma once
#include <engine.h>
#include "shell_camera.h"
#include "menu_handler.h"
#include "energy_trap_rays.h"
#include "beacon.h"
#include "game_manager.h"
#include <engine/entities/shapes/heightmap.h>

class sb_mechanics_layer : public engine::layer
{
public: 
	sb_mechanics_layer();
	~sb_mechanics_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

	void place_shell_camera();
	void cycle_shell(int direction);


private:

	bool on_menu = true;

	engine::ref<engine::shader> mesh_shader; 
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_menu_background{};
	engine::ref<engine::game_object>	m_h_terrain{};

	engine::ref<engine::heightmap>	  m_heightmap; 
	float							  m_physical_terrain_height = 0.5f;


	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_tricube_material{};
	engine::ref<engine::material>		m_tetrahedron_material{};


	engine::DirectionalLight            m_directionalLight;
	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};
	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	engine::ref<shell_camera>		m_shell_camera{};
	engine::ref<menu_handler>		m_menu_handler{};
	engine::ref<game_manager>		m_game_manager{}; 
	//Testing
	
};
