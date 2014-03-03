#	include "File.h"

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		void skip( const InputStreamInterfacePtr & _stream, size_t _count )
		{
			_stream->seek( _stream->tell() + _count );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( const InputStreamInterfacePtr & _stream )
		{
			return _stream->tell() == _stream->size();
		}
	}
}