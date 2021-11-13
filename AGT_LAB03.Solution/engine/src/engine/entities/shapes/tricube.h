#pragma once
namespace engine
{
	class mesh;

	class tricube {
	public:
		tricube(std::vector<glm::vec3> vertices);

		~tricube();

		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<tricube> create(std::vector <glm::vec3>  vertices);
	private:
		std::vector<glm::vec3> m_vertices;
		ref<engine::mesh> m_mesh; 
	};
}
