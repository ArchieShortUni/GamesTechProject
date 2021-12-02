#pragma once
#include <vector>
namespace engine
{
	class mesh;

	class turret_shape {
	public:
		turret_shape(std::vector<glm::vec3> vertices);

		~turret_shape();

		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<turret_shape> create(std::vector <glm::vec3>  vertices);
	private:
		std::vector<glm::vec3> m_vertices;
		ref<engine::mesh> m_mesh;
	};
}
