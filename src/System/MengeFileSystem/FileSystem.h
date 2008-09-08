#	pragma once

#	include "Interface\FileSystemInterface.h"

namespace Menge
{
	class FileManager;

	class FileSystem
		: public FileSystemInterface
	{
	public:
		FileSystem();
		~FileSystem();

	public:
		bool inititalize( LogSystemInterface* _logSystemInterface ) override;

		void loadPath( const Text& _path ) override;
		void loadPak( const Text& _pak ) override;
		void unloadPak( const Text& _pak ) override;

		DataStreamInterface* openFile( const Text& _filename ) override;
		DataStreamInterface* createMemoryFile( void* _data, std::streamsize _size, bool _freeOnClose ) override;
		void closeStream( DataStreamInterface * _fd ) override;
		bool existFile( const Text& _filename  ) override;
		bool deleteFile( const Text& _filename ) override;

		OutStreamInterface* openOutStream( const Menge::Text& _filename, bool _binary ) override;
		void closeOutStream( OutStreamInterface* _stream ) override;

		const char * platformBundlePath() override;

		bool createFolder( const Text& _path ) override;
		bool deleteFolder( const Text& _path ) override;
		bool changeDir( const Text& _path ) override;

		bool initAppDataPath( const Text& _game ) override;
		const Menge::String& getAppDataPath() override;

	private:
		FileManager* m_fileManager;
		Text m_appDataPath;
		LogSystemInterface* m_logSystem;
		OutStreamInterface* m_logStream;
	};
}