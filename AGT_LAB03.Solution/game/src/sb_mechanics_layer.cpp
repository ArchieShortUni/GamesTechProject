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

	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/SkyboxNight/lf.bmp", true),
		  engine::texture_2d::create("assets/textures/SkyboxNight/bk.bmp", true),
		  engine::texture_2d::create("assets/textures/SkyboxNight/rt.bmp", true),
		  engine::texture_2d::create("assets/textures/SkyboxNight/ft.bmp", true),
		  engine::texture_2d::create("assets/textures/SkyboxNight/up.bmp", true),
		  engine::texture_2d::create("assets/textures/SkyboxNight/dn.bmp", true)
		});

	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.bmp", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	//TextForMenu
	std::vector<std::string> letterstoprow = { "m-8-C.obj","m-7-O.obj","m-6-V.obj","m-5-E.obj","m-4-R.obj","m-3-T.obj" };

	for (int i=0;  i < letterstoprow.size(); i++) {
		std::string path = "assets/models/static/Menu/" + letterstoprow.at(i);

		engine::ref<engine::model> letter_model = engine::model::create(path);
		engine::game_object_properties letter_props;
		letter_props.meshes = letter_model->meshes();
		letter_props.textures = letter_model->textures();
		float tree_scale = 3.f / glm::max(letter_model->size().x, glm::max(letter_model->size().y, letter_model->size().z));
		letter_props.position = { 0, 0.5f, 0.f };
		letter_props.bounding_shape = letter_model->size() / 2.f * tree_scale;
		letter_props.scale = glm::vec3(tree_scale);

		m_letter =   engine::game_object::create(letter_props);
		m_menuitems.emplace_back(m_letter); 
	}














	m_game_objects.push_back(m_terrain);


	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();
}

sb_mechanics_layer::~sb_mechanics_layer() {}

void sb_mechanics_layer::on_update(const engine::timestep& time_step) {
	m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

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


	engine::renderer::submit(mesh_shader, m_terrain);

	for (int i = 0; i < m_menuitems.size(); i++) {
		glm::mat4 letter_transform(1.0f);
		letter_transform = glm::translate(letter_transform, m_menuitems.at(i)->position());
		letter_transform = glm::rotate(letter_transform, m_menuitems.at(i)->rotation_amount(), m_menuitems.at(i)->rotation_axis());
		letter_transform = glm::translate(letter_transform, -m_menuitems.at(i)->offset() * m_menuitems.at(i)->scale());
		letter_transform = glm::scale(letter_transform, m_menuitems.at(i)->scale());

		engine::renderer::submit(mesh_shader, letter_transform, m_menuitems.at(i));
	}
	engine::renderer::end_scene();

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	std::string t =  std::to_string(m_menuitems.size());
	m_text_manager->render_text(text_shader,t, 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
}


void sb_mechanics_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{
			engine::render_command::toggle_wireframe();
		}
	}
}
