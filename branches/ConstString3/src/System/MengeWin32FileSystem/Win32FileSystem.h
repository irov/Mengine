#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include <map>

#include "WindowsLayer/WindowsLayer.h"

namespace Menge
{
	class Win32InputStream;

	class Win32FileSystem
		: public FileSystemInterface
	{
	public:
		Win32FileSystem();
		~Win32FileSystem();

    public:
        bool initialize( LogServiceInterface * _logService ) override;

	public:
		FileInputStreamInterface* createInputStream() override;
		void closeInputStream( FileInputStreamInterface * _fd ) override;
		
		FileOutputStreamInterface* createOutputStream() override;
		void closeOutputStream( FileOutputStreamInterface* _stream ) override;

		MappedFileInputStreamInterface * openMappedInputStream( const WString& _filename ) override;
		void closeMappedInputStream( FileInputStreamInterface * _fd ) override;

		bool existFile( const WString& _filename  ) const override;
		bool deleteFile( const WString& _filename ) override;

		bool createFolder( const WString& _path ) override;
		bool deleteFolder( const WString& _path ) override;

	private:
        LogServiceInterface * m_logService;

		struct FileMappingInfo
		{
			HANDLE hFile;
			HANDLE hMapping;
		};

		typedef std::map< void*, FileMappingInfo > TFileMappingMap;
		TFileMappingMap m_fileMappingMap;
	};
}
