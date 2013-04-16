#	include "File.h"
#	include "String.h"

#	include "Interface/StreamInterface.h"

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		void skip( InputStreamInterface * _stream, int _count )
		{
			_stream->seek( _stream->tell() + _count );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( InputStreamInterface * _stream )
		{
			return _stream->tell() == _stream->size();
		}
	}
}