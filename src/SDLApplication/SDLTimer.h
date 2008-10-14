#	pragma once

#	include <SDL.h>

#	include <time.h>

	class SDLTimer
	{
	private:
		struct timeval start;    

	public:
		// overrides standard methods
		void reset();
		unsigned long getMilliseconds();
		unsigned long getMicroseconds();
	};
