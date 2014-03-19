#	pragma once

#	include "Interface/StreamInterface.h"

#	include "mkvreader.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class VideoMkvReader
		: public mkvparser::IMkvReader
	{
	public:
		VideoMkvReader( const InputStreamInterfacePtr & _stream );

	protected:
		int Read( long long _offset, long _len, unsigned char * _buf ) override;
		int Length( long long * _total, long long * _available ) override;

	protected:
		InputStreamInterfacePtr m_stream;
		size_t m_size;

		long long m_pos;
	};
}