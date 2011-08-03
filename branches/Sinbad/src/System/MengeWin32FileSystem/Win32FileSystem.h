#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include <map>

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

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
		InputStreamInterface* openInputStream( const String& _filename ) override;
		void closeInputStream( InputStreamInterface * _fd ) override;

		OutputStreamInterface* openOutputStream( const Menge::String& _filename ) override;
		void closeOutputStream( OutputStreamInterface* _stream ) override;

		void* openMappedFile( const String& _filename, int* _size ) override;
		void closeMappedFile( void* _file ) override;

		bool existFile( const String& _filename  ) override;
		bool deleteFile( const String& _filename ) override;

		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;

		void correctPath( String& _path ) const;

	private:
		typedef std::vector<Win32InputStream*> TInputStreamPool;
		TInputStreamPool m_inputStreamPool;

		struct FileMappingInfo
		{
			HANDLE hFile;
			HANDLE hMapping;
		};

		typedef std::map< void*, FileMappingInfo > TFileMappingMap;
		TFileMappingMap m_fileMappingMap;
		bool m_ansiCallsOnly;
	};
}
