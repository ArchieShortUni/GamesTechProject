#include "pch.h"
#include "turret_shape.h"
#include <engine.h>

engine::turret_shape::turret_shape(std::vector<glm::vec3> vertices) : m_vertices(vertices) {
	std::vector<glm::vec3> normals;

	//LN
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(1), vertices.at(0) - vertices.at(4)));
	//LE
	normals.push_back(glm::cross(vertices.at(1) - vertices.at(2), vertices.at(1) - vertices.at(4)));
	//LS
	normals.push_back(glm::cross(vertices.at(2) - vertices.at(3), vertices.at(2) - vertices.at(5)));
	//LW
	normals.push_back(glm::cross(vertices.at(3) - vertices.at(0), vertices.at(3) - vertices.at(5)));


	//RN
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(6), vertices.at(7) - vertices.at(10)));
	//RE
	normals.push_back(glm::cross(vertices.at(8) - vertices.at(7), vertices.at(8) - vertices.at(11)));
	//RS
	normals.push_back(glm::cross(vertices.at(9) - vertices.at(8), vertices.at(9) - vertices.at(11)));
	//RW
	normals.push_back(glm::cross(vertices.at(6) - vertices.at(9), vertices.at(6) - vertices.at(10)));


	//GN
	normals.push_back(glm::cross(vertices.at(12) - vertices.at(13), vertices.at(12) - vertices.at(16)));

	//GE
	normals.push_back(glm::cross(vertices.at(13) - vertices.at(14), vertices.at(13) - vertices.at(17)));

	//GS
	normals.push_back(glm::cross(vertices.at(14) - vertices.at(15), vertices.at(14) - vertices.at(18)));

	//GW
	normals.push_back(glm::cross(vertices.at(15)- vertices.at(12), vertices.at(15) - vertices.at(19)));

	//GT
	normals.push_back(glm::cross(vertices.at(16) - vertices.at(19), vertices.at(16) - vertices.at(17)));

	//GB
	normals.push_back(glm::cross(vertices.at(12) - vertices.at(15), vertices.at(12) - vertices.at(13)));




	std::vector<mesh::vertex> turret_vertices{
		//LN
		{vertices.at(1),	normals.at(0),	{.0f,.0f}}, //0
		{vertices.at(0),	normals.at(0),	{.0f,.0f}}, //1
		{vertices.at(4),	normals.at(0),	{.0f,.0f}}, //2
		//LE
		{vertices.at(2),	normals.at(1),	{.0f,.0f}}, //3
		{vertices.at(1),	normals.at(1),	{.0f,.0f}}, //4
		{vertices.at(4),	normals.at(1),	{.0f,.0f}}, //5
		{vertices.at(5),	normals.at(1),	{.0f,.0f}}, //6
		//LS
		{vertices.at(3),	normals.at(2),	{.0f,.0f}}, //7
		{vertices.at(2),	normals.at(2),	{.0f,.0f}}, //8
		{vertices.at(5),	normals.at(2),	{.0f,.0f}}, //9
		//LW
		{vertices.at(0),	normals.at(3),	{.0f,.0f}}, //10
		{vertices.at(3),	normals.at(3),	{.0f,.0f}}, //11 
		{vertices.at(5),	normals.at(3),	{.0f,.0f}}, //12
		{vertices.at(4),	normals.at(3),	{.0f,.0f}}, //13

		//RN
		{vertices.at(7),	normals.at(4),	{.0f,.0f}}, //14
		{vertices.at(6),	normals.at(4),	{.0f,.0f}}, //15
		{vertices.at(10),	normals.at(4),	{.0f,.0f}}, //16
		//RE
		{vertices.at(7),	normals.at(5),	{.0f,.0f}}, //17
		{vertices.at(8),	normals.at(5),	{.0f,.0f}}, //18
		{vertices.at(11),	normals.at(5),	{.0f,.0f}}, //19
		{vertices.at(10),	normals.at(5),	{.0f,.0f}}, //20
		//RS
		{vertices.at(8),	normals.at(6),	{.0f,.0f}}, //21
		{vertices.at(9),	normals.at(6),	{.0f,.0f}}, //22
		{vertices.at(11),	normals.at(6),	{.0f,.0f}}, //23
		//RW
		{vertices.at(9),	normals.at(7),	{.0f,.0f}}, //24
		{vertices.at(6),	normals.at(7),	{.0f,.0f}}, //25 
		{vertices.at(10),	normals.at(7),	{.0f,.0f}}, //26
		{vertices.at(11),	normals.at(7),	{.0f,.0f}}, //27


		//GN
		{vertices.at(13),	normals.at(8),	{.0f,.0f}}, //28
		{vertices.at(12),	normals.at(8),	{.0f,.0f}}, //29 
		{vertices.at(16),	normals.at(8),	{.0f,.0f}}, //30
		{vertices.at(17),	normals.at(8),	{.0f,.0f}}, //31
		//GE
		{vertices.at(14),	normals.at(9),	{.0f,.0f}}, //32
		{vertices.at(13),	normals.at(9),	{.0f,.0f}}, //33 
		{vertices.at(17),	normals.at(9),	{.0f,.0f}}, //34
		{vertices.at(18),	normals.at(9),	{.0f,.0f}}, //35
		//GS
		{vertices.at(15),	normals.at(10),	{.0f,.0f}}, //36
		{vertices.at(14),	normals.at(10),	{.0f,.0f}}, //37 
		{vertices.at(18),	normals.at(10),	{.0f,.0f}}, //38
		{vertices.at(19),	normals.at(10),	{.0f,.0f}}, //39
		//GW
		{vertices.at(12),	normals.at(11),	{.0f,.0f}}, //40
		{vertices.at(15),	normals.at(11),	{.0f,.0f}}, //41 
		{vertices.at(19),	normals.at(11),	{.0f,.0f}}, //42
		{vertices.at(16),	normals.at(11),	{.0f,.0f}}, //43
		//GT
		{vertices.at(16),	normals.at(11),	{.0f,.0f}}, //44
		{vertices.at(19),	normals.at(11),	{.0f,.0f}}, //45 
		{vertices.at(18),	normals.at(11),	{.0f,.0f}}, //46
		{vertices.at(17),	normals.at(11),	{.0f,.0f}}, //47
		//GB
		{vertices.at(12),	normals.at(11),	{.0f,.0f}}, //48
		{vertices.at(15),	normals.at(11),	{.0f,.0f}}, //49 
		{vertices.at(14),	normals.at(11),	{.0f,.0f}}, //50
		{vertices.at(13),	normals.at(11),	{.0f,.0f}}, //51
	};

	const std::vector<uint32_t> turret_indicies{
		1,0,2,				//LN
		3,5,4,	3,6,5,		//LE
		8,7,9,				//LS
		10,12,11,	10,13,12,//LW
		14,16,15,				//RN
		17,18,19,	17,19,20,//RE
		21,22,23,						//RS
		24,25,26,	24,26,27,	//RW
		28,30,29,	28,31,30,		//GN
		32,34,33,	32,35,34,		//GE
		36,38,37,    36,39,38,							//GS
		40,42,41,	40,43,42,							//GW
		44,46,45, 44,47,46,							//GT
		48,50,49,	48,51,50							//GB
	};

	m_mesh = engine::mesh::create(turret_vertices, turret_indicies);

}

engine::turret_shape::~turret_shape() {}

engine::ref<engine::turret_shape> engine::turret_shape::create(std::vector<glm::vec3> vertices)
{
	return std::make_shared<engine::turret_shape>(vertices);
}
