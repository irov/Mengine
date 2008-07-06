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
		unsigned int size;
	};

	typedef std::vector<char> TVectorChar;

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
		DataStreamInterface * openFile( const Menge::String& _filename );
		DataStreamInterface * createMemoryFile( void * _data, std::size_t _size, bool _freeOnClose );

		FileBuffer getFileBuffer( const Menge::String& _filename );

		void closeStream( DataStreamInterface * _stream );

		OutStreamInterface * openOutStream( const String& _filename, bool _binary );
		void closeOutStream( OutStreamInterface * _outStream );

		bool changeDir( const Menge::String& _dir );

		bool initAppDataPath( const Menge::String& _game );
		const String& getAppDataPath();

	protected:
		FileSystemInterface * m_interface;

		typedef std::vector<char> TFileCache;

		TFileCache m_fileCache;
	};
}