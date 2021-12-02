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
	m_audio_manager->load_sound("assets/audio/The Mole OST - 13 The Execution.mp3", engine::sound_type::track, "menuMusic");  // Royalty free music from http://www.nosoapradio.us/
	//m_audio_manager->play("menuMusic");
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
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);



	//INITIALISE HERE 
	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties

	m_tricube_material = engine::material::create(32.0f,
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		0.3f);



	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_sealf.bmp", true),
		  engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_seabk.bmp", true),
		  engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_seart.bmp", true),
		  engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_seaft.bmp", true),
		  engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_seaup.bmp", true),
		  engine::texture_2d::create("assets/textures/skyboxBeach/lost_at_seadn.bmp", true)
		});

	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/ocean_terrain.bmp", false) };
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

	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 0.f, 0.5f, 20.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);





	engine::ref<engine::model> claw_model = engine::model::create("assets/models/static/claw.obj");
	engine::game_object_properties claw_props;

	claw_props.meshes = claw_model->meshes();
	claw_props.textures = claw_model->textures();
	float claw_scale = 3.f / glm::max(claw_model->size().x, glm::max(claw_model->size().y, claw_model->size().z));
	claw_scale /= 5;
	claw_props.scale = glm::vec3(claw_scale);

	m_claw = engine::game_object::create(claw_props);


	//PRIMITIVE OBJECT
	std::vector<glm::vec3> tricube_vertices;
	//Rectange Vertices
	tricube_vertices.push_back(glm::vec3(-5.f, 0.f, 5.f)); //0
	tricube_vertices.push_back(glm::vec3(5.f, 0.f, 5.f)); //1
	tricube_vertices.push_back(glm::vec3(5.f, 0.f, -5.f)); //2
	tricube_vertices.push_back(glm::vec3(-5.f, 0.f, -5.f)); //3
	tricube_vertices.push_back(glm::vec3(-5.f, 2.f, 5.f)); //4
	tricube_vertices.push_back(glm::vec3(5.f, 2.f, 5.f)); //5
	tricube_vertices.push_back(glm::vec3(5.f, 2.f, -5.f)); //6
	tricube_vertices.push_back(glm::vec3(-5.f, 2.f, -5.f)); //7
	//Triangle Tip
	tricube_vertices.push_back(glm::vec3(0.f, 15.f, 0.f)); //8
	//Base vertices
	tricube_vertices.push_back(glm::vec3(-1.f, 0.f, 1.f)); //9
	tricube_vertices.push_back(glm::vec3(1.f, 0.f, 1.f)); //10
	tricube_vertices.push_back(glm::vec3(1.f, 0.f, -1.f)); //11
	tricube_vertices.push_back(glm::vec3(-1.f, 0.f, -1.f)); //12
	tricube_vertices.push_back(glm::vec3(-1.f, -8.f, 1.f)); //13
	tricube_vertices.push_back(glm::vec3(1.f, -8.f, 1.f)); //14
	tricube_vertices.push_back(glm::vec3(1.f, -8.f, -1.f)); //15
	tricube_vertices.push_back(glm::vec3(-1.f, -8.f, -1.f)); //16

	std::vector<engine::ref<engine::texture_2d>> tricube_texture = {
		engine::texture_2d::create("assets/textures/primitive_texture_pixel.bmp",false)
	};

	engine::ref<engine::tricube> tricube_shape = engine::tricube::create(tricube_vertices);
	engine::game_object_properties tricube_props;
	tricube_props.position = { 3.f, .9f, 20.f };
	tricube_props.meshes = { tricube_shape->mesh() };
	tricube_props.textures = tricube_texture;
	tricube_props.scale /= 25;
	m_tricube = engine::game_object::create(tricube_props);

	//energy trap testing 
	tricube_props.position = { 3.f, .9f, 10.f };
	m_tricube2 = engine::game_object::create(tricube_props);

	glm::vec3 dir = glm::vec3(3.f, 0.9f, 10.f) - glm::vec3(3.f, .9f, 20.f);
	//glm::vec3 dir =  glm::vec3(3.f, .9f, 20.f)- glm::vec3(3.f, 0.9f, 10.f);

	for (uint32_t i = 0; i < 3; i++) {
		m_energy_bolts.push_back(energy_trap_rays::create(glm::vec3(3.f, .9f, 20.f), dir));
	}


//	m_game_objects.push_back(m_terrain);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_crab); 

	//m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_shell_camera = shell_camera::create(1.6f, 0.9f);

	m_menu_handler = menu_handler::create(1.6f, 0.9f);

	m_game_manager = game_manager::create(m_3d_camera, 1.6f, 0.9f);

	m_game_manager->add_to_game_objects(m_terrain);

	
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
	m_game_manager->on_update(time_step);
	//m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	for (uint32_t i = 0; i < m_energy_bolts.size(); i++) {
		m_energy_bolts.at(i)->on_update(time_step);
	}
}

void sb_mechanics_layer::on_render() {
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	// set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());


	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	if(on_menu){engine::renderer::submit(mesh_shader, m_menu_background);}
	else {engine::renderer::submit(mesh_shader, m_terrain);}




	glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, m_tree->position());
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(mesh_shader, tree_transform, m_tree);

	



	m_shell_camera->on_render3d(mesh_shader);
	m_menu_handler->on_render3d(mesh_shader);
	m_game_manager->on_render3d(mesh_shader);
	m_material->submit(mesh_shader);
	//engine::renderer::submit(mesh_shader, m_turret);
	engine::renderer::submit(mesh_shader, m_tricube);
	engine::renderer::submit(mesh_shader, m_tricube2);

	for (uint32_t i = 0; i < m_energy_bolts.size(); i++) {
		m_energy_bolts.at(i)->on_render(mesh_shader);
	}

	engine::renderer::end_scene();

	//SHELL CAMERA SECTION 
	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	m_shell_camera->on_render2d(mesh_shader);
	m_menu_handler->on_render2d(mesh_shader);
	m_game_manager->on_render2d(mesh_shader);

	engine::renderer::end_scene();

	
	
	//m_game_manager->on_render2d(mesh_shader);

	//std::string percent = std::to_string(m_beacon->get_percent());
	//percent.erase(percent.find_last_not_of('0') + 1, std::string::npos);
	//percent.pop_back();
	//std::string b_p = "Beacon 1 Percentage: "+percent;
	//m_text_manager->render_text(text_shader, b_p, 10.f, (float)engine::application::window().height() - 300.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));


	//Testing
	//std::string t = std::to_string(m_beacon->get_active_switches());
	//m_text_manager->render_text(text_shader, t, 10.f, (float)engine::application::window().height() - 400.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));

	
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

			//Testing
			if (e.key_code() == engine::key_codes::KEY_K) {
				//
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
				on_menu = false;
				m_menu_handler->swap();
}}



	}
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
