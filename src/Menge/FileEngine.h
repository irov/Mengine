#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Holder.h"

#	include <string>
#	include <vector>

namespace Menge
{
	struct FileBuffer
	{
		void * buffer;
		size_t size;
	};

	class FileEngine
	{
	public:
		FileEngine( FileSystemInterface * _interface );
		~FileEngine();

	public:
		void loadPath( const std::string& _path );
		void loadPak( const std::string& _filename );
		void unloadPak( const std::string& _filename );
		bool existFile( const std::string& _filename );
		bool createFolder( const std::string& _path );
		bool deleteFolder( const std::string& _path );

		FileDataInterface * openFile( const std::string& _filename );
		FileBuffer getFileBuffer( const std::string& _filename );

		void closeFile( FileDataInterface * _fd );
		bool changeDir( const std::string& _dir );

	protected:
		FileSystemInterface * m_interface;

		typedef std::vector<char> TFileCache;

		TFileCache m_fileCache;
	};
}