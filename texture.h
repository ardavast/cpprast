#include <SDL2/SDL_image.h>

class Texture {
public:
	int width, height;
	SDL_Surface *surface;
	Uint32 *pixels;

	Texture(const char *filename, const SDL_PixelFormat* fmt);
	~Texture(void);

	Uint32 get(int u, int v);
	Uint32 get(glm::vec2 uv);
};

Texture::Texture(const char *filename, const SDL_PixelFormat *fmt)
{
	SDL_Surface *surface0 = IMG_Load(filename);
	surface = SDL_ConvertSurface(surface0, fmt, 0);
	SDL_FreeSurface(surface0);

	pixels = (Uint32 *)surface->pixels;
	width = surface->w;
	height = surface->h;
}

Texture::~Texture(void)
{
	SDL_FreeSurface(surface);
}

Uint32 Texture::get(int u, int v)
{
	int pos = (height - 1 - v) * width + u;

	return (pixels[pos]);
}

Uint32 Texture::get(glm::vec2 uv)
{
	int u = uv[0] * width;
	int v = uv[1] * height;

	return get(u, v);
}
