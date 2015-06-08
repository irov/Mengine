#	pragma once

#	include "SDLSoundBuffer.h"

namespace Menge
{
	class SDLSoundBufferMemory
		: public SDLSoundBuffer
	{
	public:
		SDLSoundBufferMemory();
		~SDLSoundBufferMemory();
	};
}	// namespace Menge
