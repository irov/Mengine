
#	include "FileStreamOutStream.h"

#	include "Menge/Utils.h"

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
		//if( _binary )
		{
			mode |= std::ios_base::binary;
		}

		m_fstream.open( _filename, mode );
		if( m_fstream.fail() )
		{
			return false;
		}

		if( _binary == false )
		{
#if MENGE_ENDIAN == MENGE_ENDIAN_LITTLE
			uint16 bom = 0xFEFF;	// UTF-16LE
#else
			uint16 bom = 0xFFFE;	// UTF-16BE
#endif
			m_fstream.write( (const char*)&bom, 2 );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( const char* _data, std::streamsize _count )
	{
		m_fstream.write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamOutStream::write( const Menge::String& _str )
	{
		//Fixme?? maybe Unicode write??
/*#ifdef MENGE_UNICODE
		m_fstream << Utils::WToA( _str );
#else
		m_fstream << _str;
#endif*/
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