#	include "File.h"

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		void skip( const InputStreamInterfacePtr & _stream, size_t _count )
		{
			size_t pos = _stream->tell();
			size_t seek_pos = pos + _count;

			_stream->seek( seek_pos );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( const InputStreamInterfacePtr & _stream )
		{
			size_t pos = _stream->tell();
			size_t size = _stream->size();

			return pos == size;
		}
	}
}