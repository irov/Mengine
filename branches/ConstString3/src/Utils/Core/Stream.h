#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	namespace Utils
	{
		template<class T>
		static void readStream( FileInputInterface * _stream, T & _t )
		{
			_stream->read( (void *)&_t, sizeof(T) );
		}

		template<class T>
		static void readStream( FileInputInterface * _stream, T * _t, std::streamsize _size )
		{
			_stream->read( (void *)_t, sizeof(T) * _size );
		}

		static bool readStreamString( FileInputInterface * _stream, String & _str )
		{
			std::streamsize size = 0;
			readStream( _stream, size );

			if( size < 256 )
			{
				char buff[256];
				readStream( _stream, buff, size );

				_str.assign( buff, size );
			}
			else
			{
				char * buff = new char[size];
				readStream( _stream, buff, size );

				_str.assign( buff, size );

				delete [] buff;
			}

			return true;
		}
	}
}