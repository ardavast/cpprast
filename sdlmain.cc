#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "display.h"
#include "mesh.h"

int main(void)
{
	int width =  800;
	int height = 600;
	Display Display(width, height, "cpprast");
	Display.clear(glm::vec3(0.0f, 1.0f, 1.0f));
	Display.update();

	return (0);
}
