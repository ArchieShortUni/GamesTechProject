#include "pch.h"
#include "projectile.h"

projectile::projectile(engine::game_object_properties props, float origin_type){

	m_projectile = engine::game_object::create(props);
	projectile_hitbox.set_box(.1f, .1f, .1f, m_projectile->position());
}

void projectile::fire(glm::vec3 origin, glm::vec3 direction, float speed) {

	projectile_speed = speed;
	m_projectile->set_position(origin);
	m_projectile->set_forward(glm::normalize(direction));
}
void projectile::on_render(const engine::ref<engine::shader>& shader) {
	//projectile_hitbox.on_render(2.5f, 0.f, 0.f,shader);
	glm::mat4 proj_transform(1.f);
	proj_transform = glm::translate(proj_transform, m_projectile->position());
	proj_transform = glm::scale(proj_transform, m_projectile->scale());
	engine::renderer::submit(shader, proj_transform, m_projectile);
}

void projectile::on_update(const engine::timestep& time_step) {
	time_alive += 1 * time_step;
	m_projectile->set_position(m_projectile->position() + (m_projectile->forward() * (float)time_step * 3.0f * projectile_speed));
	glm::vec3 hitboxpos = m_projectile->position();
	hitboxpos.y -= .05f;
	projectile_hitbox.on_update(hitboxpos, m_projectile->rotation_amount(), m_projectile->rotation_axis());

}

engine::ref<projectile> projectile::create(engine::game_object_properties props,  float origin_type )
{
	return std::make_shared<projectile>(props, origin_type);
}
