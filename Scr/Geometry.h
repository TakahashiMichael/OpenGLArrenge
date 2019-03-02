// @file Geometry.h

#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/matrix.hpp>

//頂点データを格納する構造体
struct Vertex
{
	glm::vec3 position;		///<座標.
	glm::vec4 color;		///<色.
};



#endif // !GEOMETRY_H_INCLUDED
