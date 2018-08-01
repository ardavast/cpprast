#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

class Camera
{
public:
	Camera(void);
	glm::mat4 transform;
private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
};

Camera::Camera(void)
{
	pos = glm::vec3(0.0f, 0.0f, 3.0f);
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 V = glm::lookAt(pos, pos + front, up);
	std::cout << glm::to_string(V) << std::endl;
	glm::mat4 P = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	std::cout << glm::to_string(P) << std::endl;
	transform = V;
	//transform = P * V;
}
