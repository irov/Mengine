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
		FileInputStreamInterface* openInputStream( const WString& _filename ) override;
		void closeInputStream( FileInputStreamInterface * _fd ) override;

		FileOutputStreamInterface* openOutputStream( const WString& _filename ) override;
		void closeOutputStream( FileOutputStreamInterface* _stream ) override;

		bool existFile( const WString& _filename  ) override;
		bool deleteFile( const WString& _filename ) override;

		bool createFolder( const WString& _path ) override;
		bool deleteFolder( const WString& _path ) override;

	private:
		struct FileMappingInfo
		{
			HANDLE hFile;
			HANDLE hMapping;
		};

		typedef std::map< void*, FileMappingInfo > TFileMappingMap;
		TFileMappingMap m_fileMappingMap;
	};
}
