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
		std::size_t size;
	};

	typedef std::vector<char> TVectorChar;

	class FileEngine
	{
	public:
		FileEngine( FileSystemInterface * _interface );
		~FileEngine();

	public:
		void loadPath( const String& _path );
		void loadPak( const String& _filename );
		void unloadPak( const String& _filename );
		bool existFile( const String& _filename );
		bool deleteFile( const String& _filename );
		bool createFolder( const String& _path );
		bool deleteFolder( const String& _path );
		DataStreamInterface * openFile( const String& _filename );
		DataStreamInterface * createMemoryFile( void * _data, std::size_t _size, bool _freeOnClose );

		FileBuffer getFileBuffer( const String& _filename );

		void closeStream( DataStreamInterface * _stream );

		OutStreamInterface * openOutStream( const String& _filename, bool _binary );
		void closeOutStream( OutStreamInterface * _outStream );

		bool changeDir( const String& _dir );

		bool initAppDataPath( const Menge::String& _game );
		const String& getAppDataPath();

	protected:
		FileSystemInterface * m_interface;

		typedef std::vector<char> TFileCache;

		TFileCache m_fileCache;
	};
}