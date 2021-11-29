#include "pch.h"
#include "quad.h"

quad::quad(glm::vec2 half_extents) : m_half_extents(half_extents)
{

	std::vector<engine::mesh::vertex> quad_vertices
	{
		//                   position												normal				 tex coord       
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 1.f } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	m_mesh = engine::mesh::create(quad_vertices, quad_indices);
}

quad::~quad() {}

void quad::reset() {

	std::vector<engine::mesh::vertex> quad_vertices
	{
		//                   position												normal				 tex coord       
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 0.f } },
		{ { (-1.f * m_half_extents.x), -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 0.f } },
		{ {( -1.f * m_half_extents.x),  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 1.f } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	m_mesh = engine::mesh::create(quad_vertices, quad_indices);
}

void quad::change_percent(int percent) {
	
	std::vector<engine::mesh::vertex> quad_vertices
	{
		//                   position												normal				 tex coord       
		{ {(-1.f * m_half_extents.x) + (percent * increment), -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 0.f } },
		{ { (1.f * m_half_extents.x), -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 0.f } },
		{ {(1.f * m_half_extents.x) ,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 1.f } },
		{ {(-1.f * m_half_extents.x)+(percent * increment),  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 1.f } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	m_mesh = engine::mesh::create(quad_vertices, quad_indices);

}


engine::ref<quad> quad::create(glm::vec2 half_extents)
{
	return std::make_shared<quad>(half_extents);
}
