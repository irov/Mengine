#	pragma once

#	include "Interface/FileSystemInterface.h"
#include <CoreServices/CoreServices.h>

namespace Menge
{
	class FileSystem
		: public FileSystemInterface
	{
	public:
		FileSystem();
		~FileSystem();

	public:
		bool inititalize( LogSystemInterface* _logSystemInterface ) override;

		void loadPath( const String& _path ) override;
		void loadPak( const String& _pak ) override;
		void unloadPak( const String& _pak ) override;

		DataStreamInterface* openFile( const String& _filename ) override;
		DataStreamInterface* createMemoryFile( void* _data, std::streamsize _size ) override;
		void closeStream( DataStreamInterface * _fd ) override;
		bool existFile( const String& _filename  ) override;
		bool deleteFile( const String& _filename ) override;

		OutStreamInterface* openOutStream( const Menge::String& _filename, bool _binary ) override;
		void closeOutStream( OutStreamInterface* _stream ) override;

		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;

		bool initAppDataPath( const String& _game, bool _local ) override;
		const String& getAppDataPath() override;
		
		bool isAbsolutePath( const String& _path ) override;
		String joinPath( const String& _base, const String& _name ) override;


		static FSRef s_findParentDirRef( const String& _path, String& _rest );
	private:
		String m_appDataPath;
		LogSystemInterface* m_logSystem;
		OutStreamInterface* m_logStream;
		String m_initPath;
	};
}