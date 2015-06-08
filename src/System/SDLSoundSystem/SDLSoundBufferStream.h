#	pragma once

#	include "SDLSoundBuffer.h"

namespace Menge
{
	class SDLSoundBufferStream
		: public SDLSoundBuffer
	{
	public:
		SDLSoundBufferStream();
		~SDLSoundBufferStream();

	public:
		bool update() override;
	};
	
}	// namespace Menge
