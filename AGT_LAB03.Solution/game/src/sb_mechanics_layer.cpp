#include "sb_mechanics_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"


sb_mechanics_layer::sb_mechanics_layer() :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height()) {

	engine::application::window().hide_mouse_cursor();


	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/menu_music.mp3", engine::sound_type::track, "menu_music");
	m_audio_manager->load_sound("assets/audio/level_music2.mp3", engine::sound_type::track, "level_music");
	m_audio_manager->load_sound("assets/audio/musket_shot.wav", engine::sound_type::spatialised, "gun_shot");
	m_audio_manager->load_sound("assets/audio/pick_up.wav", engine::sound_type::spatialised, "pick_up");
	m_audio_manager->load_sound("assets/audio/turret_click.mp3", engine::sound_type::spatialised, "turret_placed");
	m_audio_manager->play("menu_music");
	m_audio_manager->volume("menu_music", .05f);


	//m_audio_manager->pause("music");
	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
			(float)engine::application::window().height()));

	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/Simulation/tron_lf.bmp", true),
		  engine::texture_2d::create("assets/textures/Simulation/tron_bk.bmp", true),
		  engine::texture_2d::create("assets/textures/Simulation/tron_rt.bmp", true),
		  engine::texture_2d::create("assets/textures/Simulation/tron_ft.bmp", true),
		  engine::texture_2d::create("assets/textures/Simulation/tron_up.bmp", true),
		  engine::texture_2d::create("assets/textures/Simulation/tron_dn.bmp", true)
		});

	m_heightmap = engine::heightmap::create("assets/textures/heightmap.bmp", "assets/textures/Terrain.bmp", 170.f, 170.f, glm::vec3(0.f, 0.5f, 0.f), 10.f);
	engine::game_object_properties h_terrain_props;
	//terrain_props.meshes = { terrain_shape->mesh() };
	//terrain_props.textures = terrain_textures;
	h_terrain_props.meshes = { m_heightmap->mesh() };
	h_terrain_props.textures = { m_heightmap->texture() };
	h_terrain_props.is_static = true;
	h_terrain_props.type = 0;
	//terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	h_terrain_props.bounding_shape = glm::vec3(m_heightmap->terrain_size().x, m_physical_terrain_height, m_heightmap->terrain_size().y);
	h_terrain_props.restitution = 0.92f;
	m_h_terrain = engine::game_object::create(h_terrain_props);


	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Terrain.bmp", false) };
	std::vector<engine::ref<engine::texture_2d>> menu_textures = { engine::texture_2d::create("assets/textures/menu.bmp", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	terrain_props.textures = menu_textures;
	m_menu_background = engine::game_object::create(terrain_props);

	m_text_manager = engine::text_manager::create();

	m_shell_camera = shell_camera::create(1.6f, 0.9f);

	m_menu_handler = menu_handler::create(1.6f, 0.9f,m_menu_background);

	m_game_manager = game_manager::create(m_3d_camera, 1.6f, 0.9f,m_audio_manager);


	m_game_manager->add_to_game_objects(m_terrain);

	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

}

sb_mechanics_layer::~sb_mechanics_layer() {}

void sb_mechanics_layer::place_shell_camera() {
	//Get camera position
	glm::vec3 pos = m_3d_camera.position();
	//Check if the player is crouched or not and set the placement height accordingly
	if (m_3d_camera.is_crouch()) {float place_height = pos.y - 0.8f;}
	else { float place_height = pos.y - 1.3f; }
	//Create a vector for the shell placement position
	glm::vec3 shell_pos = glm::vec3(pos.x, 0.5f, pos.z);
	m_shell_camera->add_shell(shell_pos);
	//Create a vector to look down on the shell and set it to the most recent shell position
	glm::vec3 new_camera_position = glm::vec3(pos.x, 20.0f, pos.z);
	m_3d_camera.update_shell_position(new_camera_position);

}

void sb_mechanics_layer::cycle_shell(int direction) {
	if (m_shell_camera->get_current_shell() == -1)
		return;
	glm::vec3 pos = m_shell_camera->cycle_pos(direction);
	glm::vec3 new_camera_position = glm::vec3(pos.x, 20.0f, pos.z);

	m_3d_camera.update_shell_position(new_camera_position);
}

void sb_mechanics_layer::on_update(const engine::timestep& time_step) {
	m_3d_camera.on_update(time_step);
	if (!on_menu) {
		m_game_manager->on_update(time_step);
	}
	else {
		m_menu_handler->on_update(time_step);
	}
}

void sb_mechanics_layer::on_render() {
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	// set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);



	m_material->submit(mesh_shader);

	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);


	if (on_menu) {
		m_menu_handler->on_render3d(mesh_shader);
		engine::renderer::submit(mesh_shader, m_menu_background);
	}
	else {
		engine::renderer::submit(mesh_shader, m_terrain);
		engine::renderer::submit(mesh_shader, m_h_terrain);
		m_game_manager->on_render3d(mesh_shader);
		m_shell_camera->on_render3d(mesh_shader);
		
	}


	
	//engine::renderer::submit(mesh_shader, m_turret);
	engine::renderer::end_scene();

	//SHELL CAMERA SECTION 
	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);

	if (on_menu) { m_menu_handler->on_render2d(mesh_shader); }
	else {
		m_shell_camera->on_render2d(mesh_shader);
		m_game_manager->on_render2d(mesh_shader);
	}

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	engine::renderer::end_scene();
}



void sb_mechanics_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (!on_menu) {
			if (e.key_code() == engine::key_codes::KEY_TAB)
			{
				engine::render_command::toggle_wireframe();
			}
			if (e.key_code() == engine::key_codes::KEY_C) {
				m_game_manager->start_crouch();
			}
			if (e.key_code() == engine::key_codes::KEY_G) {
				m_3d_camera.swap_view();
				m_shell_camera->swap();

			}
			if (e.key_code() == engine::key_codes::KEY_F) {
				place_shell_camera();
			}


			//swapping camera
			if (e.key_code() == engine::key_codes::KEY_COMMA) {
				cycle_shell(-1);
			}
			if (e.key_code() == engine::key_codes::KEY_PERIOD) {
				cycle_shell(1);
			}

			if (e.key_code() == engine::key_codes::KEY_E) {
				m_game_manager->interactStart();
			}

			if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT) {
				m_game_manager->start_sprint();
				m_3d_camera.start_sprint();
			}
		}
		else {
			if (e.key_code() == engine::key_codes::KEY_SPACE) {
				m_3d_camera.swap_view();
				m_game_manager->initialise();
				on_menu = false;
				m_menu_handler->swap();

				m_audio_manager->stop("menu_music");
				m_audio_manager->play("level_music");
				m_audio_manager->volume("level_music", .05f);

			}}}


	if (event.event_type() == engine::event_type_e::key_released) {
		auto& e = dynamic_cast<engine::key_released_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_C) {
			m_game_manager->end_crouch();
		}
		if (e.key_code() == engine::key_codes::KEY_E) {
			m_game_manager->interactEnd();
		}

		if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT) {
			m_game_manager->end_sprint();
			m_3d_camera.end_sprint();
		}
	}
}
