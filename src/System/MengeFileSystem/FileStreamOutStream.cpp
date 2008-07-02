
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
	bool FileStreamOutStream::open( const wchar_t* _filename, bool _binary )
	{
		std::ios_base::open_mode mode = std::ios_base::out;
		if( _binary )
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
	void FileStreamOutStream::write( const char* _data, int _count )
	{
		m_fstream.write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( const Menge::String& _str )
	{
		m_fstream << _str;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( int _num )
	{
		m_fstream << _num;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge