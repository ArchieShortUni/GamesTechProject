#include "pch.h"
#include "tricube.h"
#include <engine.h>

engine::tricube::tricube(std::vector<glm::vec3> vertices) : m_vertices(vertices) {
	std::vector<glm::vec3> normals;

	//N
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(1), vertices.at(0) - vertices.at(4)));
	//NT
	normals.push_back(glm::cross(vertices.at(4) - vertices.at(5), vertices.at(4) - vertices.at(8)));
	//E
	normals.push_back(glm::cross(vertices.at(1) - vertices.at(2), vertices.at(1) - vertices.at(5)));
	//ET
	normals.push_back(glm::cross(vertices.at(5) - vertices.at(6), vertices.at(5) - vertices.at(8)));
	//S
	normals.push_back(glm::cross(vertices.at(2) - vertices.at(3), vertices.at(2) - vertices.at(6)));
	//ST
	normals.push_back(glm::cross(vertices.at(6) - vertices.at(7), vertices.at(6) - vertices.at(8)));
	//W
	normals.push_back(glm::cross(vertices.at(3) - vertices.at(0), vertices.at(3) - vertices.at(7)));
	//WT
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(4), vertices.at(7) - vertices.at(8)));
	//B
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(1), vertices.at(0) - vertices.at(3)));
	//BN
	normals.push_back(glm::cross(vertices.at(9) - vertices.at(10), vertices.at(9) - vertices.at(13)));
	//BE
	normals.push_back(glm::cross(vertices.at(10) - vertices.at(11), vertices.at(10) - vertices.at(14)));
	//BS
	normals.push_back(glm::cross(vertices.at(11) - vertices.at(12), vertices.at(11) - vertices.at(15)));
	//BW
	normals.push_back(glm::cross(vertices.at(12) - vertices.at(9), vertices.at(12) - vertices.at(16)));
	//BB
	normals.push_back(glm::cross(vertices.at(13) - vertices.at(14), vertices.at(13) - vertices.at(16)));

	std::vector<mesh::vertex> tricube_vertices
	{
		//N
		{vertices.at(1),	normals.at(0),	{0.f,0.f}},	//0
		{vertices.at(0),	normals.at(0),	{1.f,0.f}},	//1
		{vertices.at(4),	normals.at(0),	{1.f,1.f}},	//2
		{vertices.at(5),	normals.at(0),	{0.f,1.f}},	//3

		//E
		{vertices.at(2),	normals.at(2),	{0.f,0.f}}, //4
		{vertices.at(1),	normals.at(2),	{1.f,0.f}}, //5
		{vertices.at(5),	normals.at(2),	{1.f,1.f}}, //6
		{vertices.at(6),	normals.at(2),	{0.f,1.f}}, //7

		//S
		{vertices.at(3),	normals.at(4),	{0.f,0.f}}, //8
		{vertices.at(2),	normals.at(4),	{1.f,0.f}}, //9 
		{vertices.at(6),	normals.at(4),	{1.f,1.f}}, //10
		{vertices.at(7),	normals.at(4),	{0.f,1.f}}, //11

		//W
		{vertices.at(0),	normals.at(6),	{0.f,0.f}}, //12
		{vertices.at(3),	normals.at(6),	{1.f,0.f}}, //13
		{vertices.at(7),	normals.at(6),	{1.f,1.f}}, //14
		{vertices.at(4),	normals.at(6),	{0.f,1.f}}, //15

		//NT
		{vertices.at(5),	normals.at(1),	{0.f,0.f}}, //16
		{vertices.at(4),	normals.at(1),	{1.f,0.f}}, //17
		{vertices.at(8),	normals.at(1),	{.5f,1.f}}, //18

		//ET
		{vertices.at(6),	normals.at(3),	{0.f,0.f}}, //19
		{vertices.at(5),	normals.at(3),	{1.f,0.f}}, //20
		{vertices.at(8),	normals.at(3),	{.5f,1.f}}, //21

		//ST
		{vertices.at(7),	normals.at(5),	{0.f,0.f}}, //22
		{vertices.at(6),	normals.at(5),	{1.f,0.f}}, //23
		{vertices.at(8),	normals.at(5),	{.5f,1.f}}, //24

		//WT
		{vertices.at(4),	normals.at(7),	{0.f,0.f}}, //25
		{vertices.at(7),	normals.at(7),	{1.f,0.f}}, //26
		{vertices.at(8),	normals.at(7),	{.5f,1.f}}, //27

		//B
		{vertices.at(3),	normals.at(8),	{0.f,0.f}}, //28
		{vertices.at(0),	normals.at(8),	{1.f,0.f}}, //29
		{vertices.at(1),	normals.at(8),	{1.f,1.f}}, //30
		{vertices.at(2),	normals.at(8),	{0.f,1.f}}, //31

		//BN
		{vertices.at(14),	normals.at(9),	{0.f,0.f}}, //32
		{vertices.at(13),	normals.at(9),	{1.f,0.f}}, //33
		{vertices.at(9),	normals.at(9),	{1.f,1.f}}, //34
		{vertices.at(10),	normals.at(9),	{0.f,1.f}}, //35

		//BE
		{vertices.at(15),	normals.at(10),	{0.f,0.f}}, //36
		{vertices.at(14),	normals.at(10),	{1.f,0.f}}, //37
		{vertices.at(10),	normals.at(10),	{1.f,1.f}}, //38
		{vertices.at(11),	normals.at(10),	{0.f,1.f}}, //39

		//BS
		{vertices.at(16),	normals.at(11),	{0.f,0.f}}, //40
		{vertices.at(15),	normals.at(11),	{1.f,0.f}}, //41
		{vertices.at(11),	normals.at(11),	{1.f,1.f}}, //42
		{vertices.at(12),	normals.at(11),	{0.f,1.f}}, //43

		//BW
		{vertices.at(13),	normals.at(12),	{0.f,0.f}}, //44
		{vertices.at(16),	normals.at(12),	{1.f,0.f}}, //45
		{vertices.at(12),	normals.at(12),	{1.f,1.f}}, //46
		{vertices.at(9),	normals.at(12),	{0.f,1.f}}, //47

		//BB
		{vertices.at(16),	normals.at(11),	{0.f,0.f}}, //48
		{vertices.at(13),	normals.at(11),	{1.f,0.f}}, //49
		{vertices.at(14),	normals.at(11),	{1.f,1.f}}, //50
		{vertices.at(15),	normals.at(11),	{0.f,1.f}}, //51

	};

	const std::vector<uint32_t> tricube_indicies{
		0,2,1,	0,3,2,			//N
		4,6,5,	4,7,6,			//E
		8,10,9, 8,11,10,		//S
		12,14,13, 12,15,14,		//E
		16,18,17,				//NT
		19,21,20,				//ET
		22,24,23,				//ST
		25,27,26,				//WT
		28,30,29, 28,31,30,		//B
		32,34,33,	32,35,34,	//BN
		36,38,37,	36,39,38,	//BE
		40,42,41,	40,43,42,	//BS
		44,46,45,	44,47,46,	//BW
		48,50,49,	48,51,50	//BB


	};	

	m_mesh = engine::mesh::create(tricube_vertices, tricube_indicies);

}

engine::tricube::~tricube() {}

engine::ref<engine::tricube> engine::tricube::create(std::vector<glm::vec3> vertices)
{
	return std::make_shared<engine::tricube>(vertices);
}

