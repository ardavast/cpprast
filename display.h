#include <stdio.h>
#include <cstring>

#include <SDL2/SDL.h>


class Display {
public:
	int width, height;
	const char *title0;
	char *title;
	size_t title_len;
	SDL_Window *window;
	SDL_Surface *surface;
	const SDL_PixelFormat *fmt;
	Uint32 *pixels;

	Display(int width, int height, const char *title);
	~Display(void);

	void update();
	void update_title(Uint32 ms);
	void clear(Uint8 r, Uint8 g, Uint8 b);
	void pixel(int x, int y, Uint8 r, Uint8 g, Uint8 b);
	void pixel(int x, int y, Uint32 color);
};

Display::Display(int width, int height, const char *title)
{
	this->width = width;
	this->height = height;

	title0 = title;
	title_len = sizeof(char) * (strlen(title) + 16);
	this->title = (char *)malloc(title_len);
	memset(this->title, '\0', title_len);
	snprintf(this->title, title_len, "%s", title);

	window = SDL_CreateWindow(
	  this->title,
	  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	  this->width, this->height,
	  SDL_WINDOW_SHOWN);

	surface = SDL_GetWindowSurface(this->window);
	fmt = surface->format;
	pixels = (Uint32 *)surface->pixels;
}

Display::~Display(void)
{
	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

void Display::update()
{
	SDL_UpdateWindowSurface(this->window);
}

void Display::update_title(Uint32 ms)
{
	if (ms == 0) {
		ms = 1;
	}

	snprintf(this->title, title_len, "%s [%dms %d fps]", title0, ms, 1000/ms);
	SDL_SetWindowTitle(window, this->title);
}

void Display::clear(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
}

void Display::pixel(int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	int pos = (height - 1 - y) * width + x;
	pixels[pos] = SDL_MapRGB(surface->format, r, g, b);
}

void Display::pixel(int x, int y, Uint32 color)
{
	int pos = (height - 1 - y) * width + x;
	pixels[pos] = color;
}
