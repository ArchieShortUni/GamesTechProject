#pragma once
#include <engine.h>

class sb_mechanics_layer : public engine::layer
{
public: 
	sb_mechanics_layer();
	~sb_mechanics_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;


private:

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_crab{};

	engine::ref<engine::game_object> m_letter{};

	std::vector<engine::ref<engine::game_object>> m_menuitems{}; 

	engine::ref<engine::material>		m_material{}; 
	engine::DirectionalLight            m_directionalLight;
	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};
	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

};
