#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <vector>

#include <glm/glm.hpp>

struct vertex {
	glm::vec3 pos;
	glm::vec2 uv;
};

class Mesh {
public:
	std::vector<vertex> vertices;

	Mesh(std::string const pathname);
};

#endif // _MESH_H_
