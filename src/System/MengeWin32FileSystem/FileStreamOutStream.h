#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include <fstream>
namespace Menge
{
	class FileStreamOutStream
		: public OutputStreamInterface
	{
	public:
		FileStreamOutStream();
		~FileStreamOutStream();

		bool open( const char* _filename );
		void write( const void * _data, std::streamsize _count ) override;
		void flush() override;

	protected:
		std::ofstream m_fstream;
	};
}	// namespace Menge
