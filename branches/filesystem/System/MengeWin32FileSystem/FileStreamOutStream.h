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

		bool open( const char* _filename, bool _binary );
		void write( const void * _data, std::streamsize _count ) override;
		void write( const Menge::String& _str ) override;
		void write( int _num ) override;
		void write( float _num ) override;
		void flush() override;

	protected:
		std::ofstream m_fstream;
	};
}	// namespace Menge
