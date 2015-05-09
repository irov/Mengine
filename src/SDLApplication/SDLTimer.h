#   pragma once

#   include "Interface/PlatformInterface.h"

#	include "SDL.h"

namespace Menge
{
    class SDLTimer
        : public TimerInterface
    {
    public:
		SDLTimer();
		~SDLTimer();

    public:
        void initialize() override;
        void reset() override;

	public:
        float getDeltaTime() const override;
		uint64_t getUnixTime() const override;

    public:
        uint64_t getMilliseconds() override;
        uint64_t getMicroseconds() override;

    protected:
		mutable Uint32 m_time;
    };
}