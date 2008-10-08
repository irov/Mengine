
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
	bool FileStreamOutStream::open( const char* _filename, bool _binary )
	{
		std::ios_base::open_mode mode = std::ios_base::out;
		//if( _binary )
		{
			mode |= std::ios_base::binary;
		}

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
	void FileStreamOutStream::write( const Menge::String& _str )
	{
		write( (const char*)_str.c_str(), _str.size() * sizeof(TChar) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( int _num )
	{
		m_fstream << _num;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::flush()
	{
		m_fstream.flush();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge