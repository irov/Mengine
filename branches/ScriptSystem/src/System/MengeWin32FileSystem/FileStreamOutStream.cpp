
#	include "FileStreamOutStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStreamOutStream::FileStreamOutStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileStreamOutStream::~FileStreamOutStream()
	{
		if( m_fstream.is_open() )
		{
			m_fstream.close();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStreamOutStream::open( const wchar_t* _filename )
	{
		std::ios_base::openmode mode = std::ios_base::out;
		mode |= std::ios_base::binary;

		m_fstream.open( _filename, mode );
		if( m_fstream.fail() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( const void* _data, std::streamsize _count )
	{
		m_fstream.write( (const char *)_data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::flush()
	{
		m_fstream.flush();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
