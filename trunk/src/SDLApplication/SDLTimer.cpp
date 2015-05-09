#   include "SDLTimer.h"

#	include <ctime>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	SDLTimer::SDLTimer()
        : m_time(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	SDLTimer::~SDLTimer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLTimer::initialize()
    {
        this->reset();
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLTimer::reset()
    {
		Uint32 time = SDL_GetTicks();

        m_time = time;
    }
    //////////////////////////////////////////////////////////////////////////
	float SDLTimer::getDeltaTime() const
    {
		Uint32 curTime = SDL_GetTicks();
		Uint32 delta = curTime - m_time;

        m_time = curTime;

        return static_cast<float>(delta);
    }
	//////////////////////////////////////////////////////////////////////////
	uint64_t SDLTimer::getUnixTime() const
	{
		uint64_t t = std::time(nullptr);

		return t;
	}
    //////////////////////////////////////////////////////////////////////////
	uint64_t SDLTimer::getMilliseconds()
    {
		Uint32 ms = SDL_GetTicks();
        uint64_t ul_ms = static_cast<uint64_t>(ms);

        return ul_ms;
    }
    //////////////////////////////////////////////////////////////////////////
	uint64_t SDLTimer::getMicroseconds()
    {
		uint64_t ms = this->getMilliseconds();
        uint64_t mcs = ms * 1000;
        
		return mcs;
    }
}