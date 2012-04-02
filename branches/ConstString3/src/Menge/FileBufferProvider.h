#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	typedef std::list<TBlobject> TBuffers;

	class FileBufferProvider
	{
	public:
		TBlobject * getBuffer();
		void releaseBuffer( TBlobject * _buffer );
		
	protected:
		TBuffers m_buffers;
		TBuffers m_free;
	};
}