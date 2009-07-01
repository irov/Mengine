#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class FileStream;

	class FileSystem
		: public FileSystemInterface
	{
	public:
		FileSystem();
		~FileSystem();

	public:
		bool inititalize( LogSystemInterface* _logSystemInterface ) override;

		DataStreamInterface* openFile( const String& _filename, bool _map = false ) override;
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

		void correctPath( String& _path ) const;

	private:
		String m_appDataPath;
		LogSystemInterface* m_logSystem;
		String m_initPath;
	};
}
