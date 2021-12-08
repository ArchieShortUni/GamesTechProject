#include "pch.h"
#include "pickup_shape.h"
#include <engine.h>		

engine::pickup_shape::pickup_shape(std::vector<glm::vec3> vertices) : m_vertices(vertices)
{
	std::vector<glm::vec3> normals;
	//TNE
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(2), vertices.at(0) - vertices.at(1)));

	//TE
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(3), vertices.at(0) - vertices.at(2)));

	//TSE
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(4), vertices.at(0) - vertices.at(3)));

	//TSW
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(5), vertices.at(0) - vertices.at(4)));

	//TW
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(6), vertices.at(0) - vertices.at(5)));

	//TNW
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(1), vertices.at(0) - vertices.at(6)));

	//BNE
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(2), vertices.at(7) - vertices.at(1)));

	//BE
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(3), vertices.at(7) - vertices.at(2)));

	//BSE
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(4), vertices.at(7) - vertices.at(3)));

	//BSW
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(5), vertices.at(7) - vertices.at(4)));

	//BW
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(6), vertices.at(7) - vertices.at(5)));

	//BNW
	normals.push_back(glm::cross(vertices.at(7) - vertices.at(1), vertices.at(7) - vertices.at(6)));


	std::vector<mesh::vertex> pickup_vertices
	{
	
		//  position			normal			      tex coord       
	
		//TNE
		{ vertices.at(0),		normals.at(0),		{ 0.5f,  1.f } },//0
		{ vertices.at(2),		normals.at(0),		{ 1.f,  0.f } },//1
		{ vertices.at(1),		normals.at(0),		{ 0.f, 0.f } },//2
		//TE
		{ vertices.at(0),		normals.at(1),		{ 0.5f,  1.f } },//3
		{ vertices.at(3),		normals.at(1),		{ 1.f,  0.f } },//4
		{ vertices.at(2),		normals.at(1),		{ 0.f, 0.f } },//5
		//TSE
		{ vertices.at(0),		normals.at(2),		{ 0.5f,  1.f } },//6
		{ vertices.at(4),		normals.at(2),		{ 1.f,  0.f } },//7
		{ vertices.at(3),		normals.at(2),		{ 0.f, 0.f } },//8
		//TSW
		{ vertices.at(0),		normals.at(3),		{ 0.5f,  1.f } },//9
		{ vertices.at(5),		normals.at(3),		{ 1.f,  0.f } },//10
		{ vertices.at(4),		normals.at(3),		{ 0.f, 0.f } },//11
		//TW
		{ vertices.at(0),		normals.at(4),		{ 0.5f,  1.f } },//12
		{ vertices.at(6),		normals.at(4),		{ 1.f,  0.f } },//13
		{ vertices.at(5),		normals.at(4),		{ 0.f, 0.f } },//14
		//TNW
		{ vertices.at(0),		normals.at(5),		{ 0.5f,  1.f } },//15
		{ vertices.at(1),		normals.at(5),		{ 1.f,  0.f } },//16
		{ vertices.at(6),		normals.at(5),		{ 0.f, 0.f } },//17

		/////
			//BNE
		{ vertices.at(7),		normals.at(11),		{ 0.5f,  1.f } },//18
		{ vertices.at(2),		normals.at(11),		{ 1.f,  0.f } },//19
		{ vertices.at(1),		normals.at(11),		{ 0.f, 0.f } },//20
		//BE
		{ vertices.at(7),		normals.at(10),		{ 0.5f,  1.f } },//21
		{ vertices.at(3),		normals.at(10),		{ 1.f,  0.f } },//22
		{ vertices.at(2),		normals.at(10),		{ 0.f, 0.f } },//23
		//BSE
		{ vertices.at(7),		normals.at(9),		{ 0.5f,  1.f } },//24
		{ vertices.at(4),		normals.at(9),		{ 1.f,  0.f } },//25
		{ vertices.at(3),		normals.at(9),		{ 0.f, 0.f } },//26
		//BSW
		{ vertices.at(7),		normals.at(8),		{ 0.5f,  1.f } },//27
		{ vertices.at(5),		normals.at(8),		{ 1.f,  0.f } },//28
		{ vertices.at(4),		normals.at(8),		{ 0.f, 0.f } },//29
		//BW
		{ vertices.at(7),		normals.at(7),		{ 0.5f,  1.f } },//30
		{ vertices.at(6),		normals.at(7),		{ 1.f,  0.f } },//31
		{ vertices.at(5),		normals.at(7),		{ 0.f, 0.f } },//32
		//BNW
		{ vertices.at(7),		normals.at(6),		{ 0.5f,  1.f } },//33
		{ vertices.at(1),		normals.at(6),		{ 1.f,  0.f } },//34
		{ vertices.at(6),		normals.at(6),		{ 0.f, 0.f } },//35


	};

	const std::vector<uint32_t> pickup_indices
	{
		0, 1 ,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,

		18,20,19,
		21,23,22,
		24,26,25,
		27,29,28,
		30,32,31,
		33,35,34
		
	};

	m_mesh = engine::mesh::create(pickup_vertices, pickup_indices);
}

engine::pickup_shape::~pickup_shape() {}

engine::ref<engine::pickup_shape> engine::pickup_shape::create(std::vector<glm::vec3> vertices)
{
	return std::make_shared<engine::pickup_shape>(vertices);
}
