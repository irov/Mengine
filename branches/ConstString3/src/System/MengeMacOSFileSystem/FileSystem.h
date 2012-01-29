#	pragma once

#	include "Interface/FileSystemInterface.h"
#   include <CoreServices/CoreServices.h>

namespace Menge
{
	class FileSystem
		: public FileSystemInterface
	{
	public:
		FileSystem();
		~FileSystem();


    public
		InputStreamInterface * openInputStream( const String& _filename ) override;
		void closeInputStream( InputStreamInterface * _fd ) override;

		FileOutputInterface* openOutputStream( const Menge::String& _filename ) override;
		void closeOutputStream( FileOutputInterface* _stream ) override;

        bool existFile( const String& _filename  ) override;
		bool deleteFile( const String& _filename ) override;
        
		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;

		static FSRef s_findParentDirRef( const String& _path, String& _rest );

	private:
		String m_appDataPath;
		String m_initPath;
	};
}