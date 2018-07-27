#include <CImg.h>
#include <glm/glm.hpp>

#include "mesh.h"

using namespace cimg_library;

static glm::ivec3 w2s_pos(CImg<unsigned char> &img, glm::vec3 w_pos);
static glm::ivec2 w2s_uv(CImg<unsigned char> &img, glm::vec2 w_uv);

static inline float zbuffer_get(CImg<unsigned char> &img, float *zbuffer, int x, int y);
static inline void zbuffer_set(CImg<unsigned char> &img, float *zbuffer, int x, int y, float value);

static glm::vec3 barycentric(glm::vec3 sp0, glm::vec3 sp1, glm::vec3 sp2, glm::vec3 P);
static inline float interp(glm::vec3 bc_screen, float v0, float v1, float v2);
static inline glm::vec2 interp(glm::vec3 bc_screen, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2);
static void triangle(CImg<unsigned char> &img, vertex v0, vertex v1, vertex v2,
                     float *zbuffer, CImg<unsigned char> &texture);

int main(void)
{
	int width = 800;
	int height = 800;
	CImg<unsigned char> img(width, height, 1, 3, 0);
	CImg<float> zb(width, height, 1, 1, -1.0f);
	Mesh mesh("african_head.obj");
	CImg<unsigned char> diffuse("african_head_diffuse.tga");
	diffuse.mirror('y');

	float *zbuffer = (float *)malloc(sizeof(float) * width * height);
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = -1.0f;
	}

	for (int i = 0; i < mesh.vertices.size(); i += 3) {
		triangle(img, mesh.vertices[i],
		              mesh.vertices[i + 1],
		              mesh.vertices[i + 2], zbuffer, diffuse);
	}

	img.mirror('y');
	img.save_png("test.png");

	return (0);
}

static glm::ivec3 w2s_pos(CImg<unsigned char> &img, glm::vec3 w_pos)
{
	glm::ivec3 s_pos;
	s_pos.x = int((w_pos.x + 1.0) * ((img.width() - 1.0) / 2.0));
	s_pos.y = int((w_pos.y + 1.0) * ((img.height() - 1.0) / 2.0));
	return s_pos;
}

static glm::ivec2 w2s_uv(CImg<unsigned char> &img, glm::vec2 w_uv)
{
	glm::ivec2 s_uv;
	s_uv.x = int((w_uv.x) * (img.width() - 1.0));
	s_uv.y = int((w_uv.y) * (img.height() - 1.0));
	return s_uv;
}

static inline float zbuffer_get(CImg<unsigned char> &img, float *zbuffer, int x, int y)
{
	return zbuffer[y * img.width() + x];
}

static inline void zbuffer_set(CImg<unsigned char> &img, float *zbuffer, int x, int y, float value)
{
	zbuffer[y * img.width() + x] = value;
}

static glm::vec3 barycentric(glm::vec3 s_pos0, glm::vec3 s_pos1, glm::vec3 s_pos2, glm::vec3 p)
{
	glm::vec3 u = glm::cross(glm::vec3(s_pos2.x - s_pos0.x, s_pos1.x - s_pos0.x, s_pos0.x - p.x),
	                         glm::vec3(s_pos2.y - s_pos0.y, s_pos1.y - s_pos0.y, s_pos0.y - p.y));

	if (std::abs(u[2]) < 1) {
		return glm::vec3(-1.0f, 1.0f, 1.0f);
	}

	return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

static inline float interp(glm::vec3 bc_screen, float v0, float v1, float v2)
{
	return (v0 * bc_screen.x + v1 * bc_screen.y + v2 * bc_screen.z);
}


static inline glm::vec2 interp(glm::vec3 bc_screen, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
{
	return (v0 * bc_screen.x + v1 * bc_screen.y + v2 * bc_screen.z);
}


static void triangle(CImg<unsigned char> &img, vertex v0, vertex v1, vertex v2,
                     float *zbuffer, CImg<unsigned char> &texture)
{
	glm::ivec3 s_pos0 = w2s_pos(img, v0.pos);
	glm::ivec3 s_pos1 = w2s_pos(img, v1.pos);
	glm::ivec3 s_pos2 = w2s_pos(img, v2.pos);

	glm::ivec3 bboxmin, bboxmax;
	bboxmin.x = std::min(std::min(std::min(img.width() - 1, s_pos0.x), s_pos1.x), s_pos2.x);
	bboxmin.y = std::min(std::min(std::min(img.height() - 1, s_pos0.y), s_pos1.y), s_pos2.y);
	bboxmax.x = std::max(std::max(std::max(0, s_pos0.x), s_pos1.x), s_pos2.x);
	bboxmax.y = std::max(std::max(std::max(0, s_pos0.y), s_pos1.y), s_pos2.y);

	glm::ivec3 p;
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++) {
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++) {
			glm::vec3 bc_screen = barycentric(s_pos0, s_pos1, s_pos2, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
				continue;
			}

			float z = interp(bc_screen, v0.pos.z, v1.pos.z, v2.pos.z);
			if (z > zbuffer_get(img, zbuffer, p.x, p.y)) {
				zbuffer_set(img, zbuffer, p.x, p.y, z);
				glm::ivec2 uv = w2s_uv(texture, interp(bc_screen, v0.uv, v1.uv, v2.uv));

				unsigned char color[3];
				color[0] = texture(uv[0], uv[1], 0, 0);
				color[1] = texture(uv[0], uv[1], 0, 1);
				color[2] = texture(uv[0], uv[1], 0, 2);
				img.draw_point(p.x, p.y, color);
			}
		}
	}
}
