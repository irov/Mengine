#	include "File.h"
#	include "String.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		const std::streamsize stream_temp_size = 128;
		//////////////////////////////////////////////////////////////////////////
		void skip( FileInputInterface* _file, int _count )
		{
			_file->seek( _file->tell() + _count );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( FileInputInterface* _file )
		{
			return _file->tell() == _file->size();
		}
		//////////////////////////////////////////////////////////////////////////
		String getLine( FileInputInterface* _file, bool _trimAfter /*= true */ )
		{
			TChar tmpBuf[stream_temp_size];
			String retString;
			std::size_t readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _file->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				std::size_t term = readCount;
				// Terminate string
				tmpBuf[term] = '\0';

				TChar* p = StdStrchr( tmpBuf, '\n' );
				if ( p != 0 )
				{
					// Reposition backwards
					skip( _file, (int)( ( p + 1 - tmpBuf ) - readCount ) );
					*p = MENGE_TEXT('\0');
				}

				retString += tmpBuf;

				if ( p != 0 )
				{
					// Trim off trailing CR if this was a CR/LF entry
					if ( retString.length() && retString[retString.length()-1] == '\r' )
					{
						retString.erase( retString.length() - 1, 1 );
					}

					// Found terminator, break out
					break;
				}
			}

			if( _trimAfter )
			{
				trim( retString );
			}

			return retString;
		}
		//////////////////////////////////////////////////////////////////////////
		int skipLine( FileInputInterface* _file, const String& _delim /*= "\n" */ )
		{
			TChar tmpBuf[stream_temp_size];
			int total = 0;
			int readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _file->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				// Terminate string
				int term = readCount / sizeof(TChar);
				tmpBuf[term] = '\0';

				// Find first delimiter

				int pos = strcspn( tmpBuf, _delim.c_str() );

				if( pos < term )
				{
					// Found terminator, reposition backwards
					int rep = ( pos + 1 - term );
					skip( _file, rep );

					total += ( pos + 1 );

					// break out
					break;
				}

				total += readCount;
			}

			return total;
		}
		//////////////////////////////////////////////////////////////////////////
		bool getFileExt( String & _out, const String & _filename )
		{
			std::size_t pos = _filename.find_last_of( "." );

			if( pos == String::npos )
			{
				return false;
			}

			_out = _filename.substr( pos + 1, _filename.length() );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void fileWrite( FileOutputInterface* _file, const String& _string )
		{
			_file->write( _string.c_str(), _string.size() );
		}
		//////////////////////////////////////////////////////////////////////////
		void collapsePath( const String& _path, String & _collapsedPath )
		{
			// Don't ignore empty: we do want to keep trailing slashes.
			TVectorString as;

			Utils::split( as, _path, false, "/" );

			// pass to remove duplicate '/' ( .//foo, ..//foo )
			for( TVectorString::size_type i = 0
				; i < as.size()
				; /**/ )
			{
				if( as[i].empty() == true && i > 0 && ( as[i-1] == ".." || as[i-1] == "." ) )
				{
					as.erase( as.begin() + i );
				}
				else
				{
					++i;
				}
			}

			for( TVectorString::size_type i = 0; i < as.size(); ++i )
			{
				if( as[i] == ".." && i != 0 )
				{
					/* If the previous element is also "..", then we have a path beginning
					* with multiple "../"--one .. can't eat another .., since that would
					* cause "../../foo" to become "foo". */
					if( as[i-1] != ".." )
					{
						as.erase( as.begin()+i-1 );
						as.erase( as.begin()+i-1 );
						i -= 2;
					}
				}
				else if( as[i] == "" && i != 0 && i != 1 && i+1 < as.size() )
				{
					/* Remove empty parts that aren't at the beginning or end;
					* "foo//bar/" -> "foo/bar/", but "/foo" -> "/foo" and "foo/"
					* to "foo/". */
					as.erase( as.begin()+i );
					i -= 1;
				}
				else if( as[i] == "." )
				{
					as.erase( as.begin()+i );
					i -= 1;
				}
			}
			join( "/", as, _collapsedPath );
		}
	}
}