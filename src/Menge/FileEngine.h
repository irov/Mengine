#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Holder.h"

#	include <string>
#	include <vector>
#	include <map>

namespace Menge
{
	struct FileBuffer
	{
		void * buffer;
		std::streamsize size;
	};

	typedef std::vector<char> Blobject;

	class FilePack;

	class FileEngine
	{
	public:
		FileEngine( FileSystemInterface * _interface );
		~FileEngine();

	public:
		void loadPath( const String& _path );
		void loadPack( const String& _filename );
		void unloadPack( const String& _filename );
		bool existFile( const String& _filename );
		bool deleteFile( const String& _filename );
		bool createFolder( const String& _path );
		bool deleteFolder( const String& _path );
		DataStreamInterface * openFile( const String& _filename );
		DataStreamInterface * createMemoryFile( void * _data, std::streamsize _size );

		FileBuffer getFileBuffer( const String& _filename );

		void closeStream( DataStreamInterface * _stream );

		OutStreamInterface * openOutStream( const String& _filename, bool _binary );
		void closeOutStream( OutStreamInterface * _outStream );

		bool initAppDataPath( const String& _game, bool _local );
		const String& getAppDataPath();

		bool isAbsolutePath( const String& _path );
		String joinPath( const String& _base, const String& _name );

	protected:
		FileSystemInterface * m_interface;

		Blobject m_fileCache;

		struct PackInfo
		{
			FilePack* pack;
			DataStreamInterface* stream;
		};
		typedef std::map< String, PackInfo > TFilePackMap;
		TFilePackMap m_packs;

		String m_userPath;
	};
}
