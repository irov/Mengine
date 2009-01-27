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
		std::streamsize size;
	};

	typedef std::vector<char> Blobject;

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
		DataStreamInterface * createMemoryFile( void * _data, std::streamsize _size, bool _freeOnClose );

		FileBuffer getFileBuffer( const String& _filename );

		void closeStream( DataStreamInterface * _stream );

		OutStreamInterface * openOutStream( const String& _filename, bool _binary );
		void closeOutStream( OutStreamInterface * _outStream );

		bool initAppDataPath( const String& _game );
		const String& getAppDataPath();

		bool isAbsolutePath( const String& _path );
		String joinPath( const String& _base, const String& _name );
		bool isFile( const String& _filename );

	protected:
		FileSystemInterface * m_interface;

		Blobject m_fileCache;
	};
}
