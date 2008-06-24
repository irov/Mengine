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

		void loadPath( const String& _path ) override;
		void loadPak( const String& _pak ) override;
		void unloadPak( const String& _pak ) override;

		DataStreamInterface* openFile( const String& _filename ) override;
		DataStreamInterface* createMemoryFile( void* _data, std::size_t _size, bool _freeOnClose ) override;
		void closeStream( DataStreamInterface * _fd ) override;
		bool existFile( const String& _filename  ) override;

		const char * platformBundlePath() override;

		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;
		bool changeDir( const String& _path ) override;

		bool initAppDataPath( const String& _game ) override;
		const Menge::String& getAppDataPath() override;

	private:
		FileManager* m_fileManager;
		String m_appDataPath;
	};
}