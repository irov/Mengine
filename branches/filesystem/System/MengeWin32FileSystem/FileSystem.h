#	pragma once

#	include "Interface/FileSystemInterface.h"

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

		bool existFile( const String& _filename  ) override;
		bool deleteFile( const String& _filename ) override;


		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;

		void correctPath( String& _path ) const;

	private:
		typedef std::vector<Win32InputStream*> TInputStreamPool;
		TInputStreamPool m_inputStreamPool;
	};
}
