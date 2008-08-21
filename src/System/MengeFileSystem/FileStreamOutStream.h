#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include <fstream>
namespace Menge
{
	class FileStreamOutStream
		: public OutStreamInterface
	{
	public:
		FileStreamOutStream();
		~FileStreamOutStream();

		bool open( const wchar_t* _filename, bool _binary );
		void write( const char * _data, std::streamsize _count ) override;
		void write( const Menge::String& _str ) override;
		void write( int _num ) override;

	protected:
		std::ofstream m_fstream;
	};
}	// namespace Menge