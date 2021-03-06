#include <algorithm>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "display.h"
#include "buffer2d.h"
#include "timer.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"

glm::ivec3 w2s_pos(Display &fb, glm::vec3 wv0);
glm::vec3 barycentric(glm::vec3 s_pos0, glm::vec3 s_pos1, glm::vec3 s_pos2, glm::vec3 p);
float interp(glm::vec3 bc_screen, float v0, float v1, float v2);
void triangle(Display &fb, Buffer2D &zb, vertex &v0, vertex &v1, vertex &v2, Texture &diffuse);

int main(void)
{
	int width = 800;
	int height = 800;

	Display fb(width, height, "cpprast");
	Buffer2D zb(width, height);
	Timer timer;
	Camera camera;

	Mesh mesh("african_head.obj");
	Texture diffuse("african_head_diffuse.tga", fb.fmt);

	glm::mat4 M = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f,
	                        0.0f, 0.5f, 0.0f, 0.0f,
	                        0.0f, 0.0f, 0.5f, 0.0f,
	                        0.0f, 0.0f, 0.0f, 1.0f);

	//glm::mat4 V = glm::mat4(1.0f, 0.0f, 0.0f,  0.0f,
	//                        0.0f, 1.0f, 0.0f,  0.0f,
	//                        0.0f, 0.0f, 1.0f,  3.0f,
	//                        0.0f, 0.0f, 0.0f,  1.0f);

	//glm::mat4 P = glm::mat4(1.0f, 0.0f,  0.0f,      0.0f,
	//                        0.0f, 1.0f,  0.0f,      0.0f,
	//                        0.0f, 0.0f,  1.0f,      0.0f,
	//                        0.0f, 0.0f, -1.0/3.0f, 1.0f);

	vertex *vbuffer = (vertex *)malloc(sizeof(vertex) * mesh.vertices.size());

	SDL_Event e;
	bool quit = false;
	for (;;) {
		if (quit) {
			break;
		}

		timer.reset();

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
			    quit = true;
			}
		}

		//glm::mat4 MVP = P * V * M;
		glm::mat4 MVP = M;

		fb.clear(0, 0, 0);
		zb.clear(-1.0f);

		//FIXME Why is this needed?
		memset(vbuffer, '\0', sizeof(vertex) * mesh.vertices.size());

		for (int i = 0; i < mesh.vertices.size() - 1; i++) {
			vbuffer[i].pos = mesh.vertices[i].pos * MVP;
			vbuffer[i].pos.x *= vbuffer[i].pos.z;
			vbuffer[i].pos.y *= vbuffer[i].pos.z;
			//vbuffer[i].pos.z /= vbuffer[i].pos.w;
			vbuffer[i].uv = mesh.vertices[i].uv;
		}

		for (int i = 0; i < mesh.vertices.size() - 1; i += 3) {
			triangle(fb, zb, vbuffer[i], vbuffer[i + 1],
			         vbuffer[i + 2], diffuse);
		}

		fb.update();
		timer.throttle(33);
		fb.update_title(timer.get());
	}

	return (0);
}

glm::ivec3 w2s_pos(Display &fb, glm::vec3 wv0) {
	glm::ivec3 sv0;
	sv0.x = int((wv0.x + 1.0) * ((fb.width - 1.0) / 2.0));
	sv0.y = int((wv0.y + 1.0) * ((fb.height - 1.0) / 2.0));
	return sv0;
}

glm::vec3 barycentric(glm::ivec3 &s_pos0, glm::ivec3 &s_pos1, glm::ivec3 &s_pos2, glm::ivec3 &p)
{
	glm::vec3 u = glm::cross(glm::vec3(s_pos2.x - s_pos0.x, s_pos1.x - s_pos0.x, s_pos0.x - p.x),
	                         glm::vec3(s_pos2.y - s_pos0.y, s_pos1.y - s_pos0.y, s_pos0.y - p.y));

	if (std::abs(u[2]) < 1) {
		return glm::vec3(-1.0f, 1.0f, 1.0f);
	}

	return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

float interp(glm::vec3 bc_screen, float v0, float v1, float v2)
{
	return (v0 * bc_screen.x + v1 * bc_screen.y + v2 * bc_screen.z);
}

glm::vec2 interp(glm::vec3 bc_screen, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
{
	return (v0 * bc_screen.x + v1 * bc_screen.y + v2 * bc_screen.z);
}

void triangle(Display &fb, Buffer2D &zb, vertex &v0, vertex &v1, vertex &v2, Texture &diffuse)
{
	glm::ivec3 s_pos0 = w2s_pos(fb, v0.pos);
	glm::ivec3 s_pos1 = w2s_pos(fb, v1.pos);
	glm::ivec3 s_pos2 = w2s_pos(fb, v2.pos);

	glm::ivec3 bboxmin, bboxmax;
	bboxmin.x = std::min({s_pos0.x, s_pos1.x, s_pos2.x, fb.width - 1});
	bboxmin.y = std::min({s_pos0.y, s_pos1.y, s_pos2.y, fb.height - 1});
	bboxmax.x = std::max({0, s_pos0.x, s_pos1.x, s_pos2.x});
	bboxmax.y = std::max({0, s_pos0.y, s_pos1.y, s_pos2.y});

	glm::ivec3 p;
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++) {
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++) {
			glm::vec3 bc_screen = barycentric(s_pos0, s_pos1, s_pos2, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
				continue;
			}

			float z = interp(bc_screen, v0.pos.z, v1.pos.z, v2.pos.z);
			if (z > zb.get(p.x, p.y)) {
				zb.set(p.x, p.y, z);

				glm::vec2 uv = interp(bc_screen, v0.uv, v1.uv, v2.uv);
				fb.pixel(p.x, p.y, diffuse.get(uv));
			}
		}
	}
}
