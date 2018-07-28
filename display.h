#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class Display {
public:
	int width, height;
	const char *title;

	SDL_Window *window;
	SDL_Surface *surface;

	Display(int width, int height, const char *title);
	~Display();

	void update();

	void clear(Uint8 r, Uint8 g, Uint8 b);
	void clear(glm::vec3 color);

	void pixel(int x, int y, Uint8 r, Uint8 g, Uint8 b);
	void pixel(int x, int y, glm::vec3 color);

private:
	Uint32 vec3_to_rgb(glm::vec3 color);
};

Display::Display(int width, int height, const char *title)
{
	this->width = width;
	this->height = height;
	this->title = title;

	this->window = SDL_CreateWindow(
	  this->title,
	  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	  this->width, this->height,
	  SDL_WINDOW_SHOWN);

	this->surface = SDL_GetWindowSurface(this->window);

	SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, 0xFF, 0xFF, 0xFF));
}

Display::~Display()
{
	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

Uint32 inline Display::vec3_to_rgb(glm::vec3 color)
{
	Uint8 r = color[0] * 255;
	Uint8 g = color[1] * 255;
	Uint8 b = color[2] * 255;

	return SDL_MapRGB(this->surface->format, r, g, b);
}

void inline Display::update()
{
	SDL_UpdateWindowSurface(this->window);
}

void inline Display::clear(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, r, g, b));
}

void inline Display::clear(glm::vec3 color)
{
	SDL_FillRect(this->surface, NULL, vec3_to_rgb(color));
}
