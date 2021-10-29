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

	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 0.f, 0.5f, 20.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	//TextForMenu
	std::vector<std::string> letterstoprow = { "m-10-C.obj","m-9-O.obj","m-8-V.obj","m-7-E.obj","m-6-R.obj","m-5-T.obj","m-0-C2.obj","m-1-R2.obj","m-4-A.obj","m-3-B.obj" };

	for (int i = 0; i < letterstoprow.size(); i++) {
		std::string path = "assets/models/static/Menu/" + letterstoprow.at(i);

		engine::ref<engine::model> letter_model = engine::model::create(path);
		engine::game_object_properties letter_props;
		letter_props.meshes = letter_model->meshes();
		letter_props.textures = letter_model->textures();
		float tree_scale = 3.f / glm::max(letter_model->size().x, glm::max(letter_model->size().y, letter_model->size().z));
		letter_props.position = { 3.0f, 2.0f, 3.5f };
		letter_props.bounding_shape = letter_model->size() / 2.f * tree_scale;
		letter_props.rotation_amount = AI_DEG_TO_RAD(270);
		letter_props.scale = glm::vec3(tree_scale);

		m_letter = engine::game_object::create(letter_props);
		m_menuitems.emplace_back(m_letter);
		//m_game_objects.emplace_back(m_letter); 
	}

	engine::ref<engine::model> crab_model = engine::model::create("assets/models/static/Menu/m-2-crab.obj");
	engine::game_object_properties crab_props;
	crab_props.meshes = crab_model->meshes();
	crab_props.textures = crab_model->textures();
	float crab_scale = 3.f / glm::max(crab_model->size().x, glm::max(crab_model->size().y, crab_model->size().z));
	crab_scale *= 2.5;
	crab_props.position = {- 2.0f,2.0f,-17.0f };
	crab_props.bounding_shape = crab_model->size() / 2.f * crab_scale;
	crab_props.rotation_amount = AI_DEG_TO_RAD(270);
	crab_props.scale = glm::vec3(crab_scale);

	m_crab = engine::game_object::create(crab_props);




	











	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_crab); 

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
		letter_transform = glm::translate(letter_transform, -m_menuitems.at(i)->offset() * m_menuitems.at(i)->scale());
		letter_transform = glm::rotate(letter_transform, m_menuitems.at(i)->rotation_amount(), m_menuitems.at(i)->rotation_axis());
		letter_transform = glm::scale(letter_transform, m_menuitems.at(i)->scale());

		engine::renderer::submit(mesh_shader, letter_transform, m_menuitems.at(i));
	}

	glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, m_tree->position());
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(mesh_shader, tree_transform, m_tree);

	glm::mat4 crab_transform(1.0f);
	crab_transform = glm::translate(crab_transform, m_crab->position());
	crab_transform = glm::rotate(crab_transform, m_crab->rotation_amount(), m_crab->rotation_axis());
	crab_transform = glm::scale(crab_transform, m_crab->scale());
	engine::renderer::submit(mesh_shader, crab_transform, m_crab);


	engine::renderer::end_scene();

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	std::string t =  std::to_string(m_menuitems.size());
	glm::vec3 ps = m_3d_camera.position();
	std::string pos = "x: " + std::to_string(ps.x) + " y: " + std::to_string(ps.y) + " z:" + std::to_string(ps.z);
	m_text_manager->render_text(text_shader,pos, 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
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
