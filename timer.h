#include <SDL2/SDL.h>

class Timer
{
public:
	Timer(void);
	void reset(void);
	void update(void);
	Uint32 get();
	void sleep(Uint32 ms);
	void throttle(Uint32 ms);

private:
	Uint32 start_time;
};

Timer::Timer(void)
{
	reset();
}

void Timer::reset(void)
{
	start_time = SDL_GetTicks();
}

Uint32 Timer::get(void)
{
	return (SDL_GetTicks() - start_time);
}

void Timer::sleep(Uint32 ms)
{
	SDL_Delay(ms);
}

void Timer::throttle(Uint32 ms)
{
	Uint32 dt = get();

	if (dt < ms) {
		sleep(ms - dt);
	}
}
