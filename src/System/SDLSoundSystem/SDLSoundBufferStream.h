#	pragma once

#	include "MarmaladeSoundBuffer.h"

namespace Menge
{
	class MarmaladeSoundBufferStream
		: public MarmaladeSoundBuffer
	{
	public:
		MarmaladeSoundBufferStream();
		~MarmaladeSoundBufferStream();

	public:
		bool update() override;
	};
	
}	// namespace Menge
