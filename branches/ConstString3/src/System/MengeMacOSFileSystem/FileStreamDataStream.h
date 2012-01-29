#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include <fstream>

namespace Menge
{
	class FileStreamDataStream 
		: public FileInputInterface
	{
	public:
		FileStreamDataStream( std::ifstream* _s, std::streamsize _size );
		~FileStreamDataStream();

	public:
		void release() override;
	
		std::streamsize read( void* _buf, std::streamsize _count ) override;

		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		int tell() const override;
		bool eof() const override;
		std::streamsize size() const;

	protected:
		/// Reference to source stream
		std::ifstream* m_stream;
		std::streamsize m_size;
	};
}