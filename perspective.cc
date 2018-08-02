#include <cstdio>
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

glm::mat4 projection;

glm::vec4 transform(glm::vec4 v)
{
	v = v * projection;
	v /= v.w;
	return (v);
}

void print_frustum(float l, float r, float b, float t, float n, float f)
{
	printf("%s\n", glm::to_string(transform(glm::vec4(l, b, -n, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(r, b, -n, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(r, t, -n, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(l, t, -n, 1.0f))).c_str());

	printf("%s\n", glm::to_string(transform(glm::vec4(l, b, -f, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(r, b, -f, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(r, t, -f, 1.0f))).c_str());
	printf("%s\n", glm::to_string(transform(glm::vec4(l, t, -f, 1.0f))).c_str());
}

int main(void)
{
	float fov = glm::radians(45.0f);
	float aspect = 4.0f/3.0f;
	float n = 0.1f;
	float f = 100.0f;
	float t = n * tanf(fov / 2.0f);
	float b = -t;
	float r = t * aspect;
	float l = -r;

	printf("l = %f r = %f b = %f t = %f n = %f f = %f\n", l, r, b, t, n, f);
	projection = glm::mat4(
	  n / r, 0.0,   0.0,                                 0.0,
	  0.0,   n / t, 0.0,                                 0.0,
	  0.0,   0.0,   - (f + n) / (f - n), - (2*f*n) / (f - n),
	  0.0,   0.0,   -1.0,                                0.0);

	print_frustum(l, r, b, t, n, f);
	puts("");

	projection = glm::frustum(l, r, b, t, n, f);
	print_frustum(l, r, b, t, n, f);
	puts("");

	projection = glm::perspective(fov, aspect, 0.1f, 100.0f);
	print_frustum(l, r, b, t, n, f);

	return (0);
}
