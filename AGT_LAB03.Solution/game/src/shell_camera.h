#pragma once
#include <engine.h>

class quad;

class shell_camera {
public:
	shell_camera(float width, float height);
	~shell_camera();


	void on_render2d(engine::ref<engine::shader> shader);
	void on_render3d(engine::ref<engine::shader> shader); 
	void swap();

	static engine::ref<shell_camera> create(float width, float height);

	void add_shell(glm::vec3 pos);

	glm::vec3 cycle_pos(int direction);

	int get_current_shell() { return current_shell; }

	int get_container_size() { return m_shells_container.size(); }

	std::string get_shell_text() {
		std::string s = "Viewing Shell: " + std::to_string(current_shell) + " / " + std::to_string(m_shells_container.size());
		return s;
	}
	
private:
	bool shell_active;
	int current_shell = -1;
	engine::game_object_properties shell_props;

	std::vector<engine::ref<engine::game_object>> m_shells_container{};

	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;


};
