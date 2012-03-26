#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/PluginInterface.h"
#	include "Utils/Core/ConstString.h"

namespace Menge
{
	class FileInputStreamInterface
		: public InputStreamInterface
	{
	public:
		virtual bool open( const String& _filename ) = 0;
		virtual void close() = 0;		
	};

	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const String& _filename ) = 0;
		virtual void close() = 0;
		virtual int tell() = 0;
	};
	
	class FileSystemInterface
	{
	public:
		virtual bool existFile( const String& _filename ) = 0;
		
		virtual FileInputStreamInterface* openInputStream( const String& _filename ) = 0;
		virtual void closeInputStream( FileInputStreamInterface* _stream ) = 0;
		
		virtual FileOutputStreamInterface* openOutputStream( const String& _filename ) = 0;
		virtual void closeOutputStream( FileOutputStreamInterface* _stream ) = 0;
		
		virtual bool deleteFile( const String& _filename ) = 0;
		virtual bool createFolder( const String& _path ) = 0;
		virtual bool deleteFolder( const String& _path ) = 0;
	};
	
	class FileServiceInterface 
		: public ServiceInterface
	{
	public:
		virtual bool mountFileSystem( const ConstString& _fileSystemName, const String& _path, const ConstString & _type, bool _create ) = 0;
		virtual void unmountFileSystem( const ConstString& _fileSystemName ) = 0;

	/*public:
		virtual bool getFullPath( const ConstString& _fileSystemName, const ConstString & _path, String & _fullPath ) const  = 0;
		virtual bool getFileSystemPath( const ConstString& _fileSystemName, String & _fileSystemPath ) const = 0 ;
	*/
	public:
		virtual bool existFile( const ConstString& _fileSystemName, const String& _filename ) const = 0;

		virtual FileInputStreamInterface * createInputFile( const ConstString& _fileSystemName ) = 0;
		virtual FileInputStreamInterface * openInputFile( const ConstString& _fileSystemName, const String& _filename ) = 0;

		virtual FileOutputStreamInterface * createOutputFile( const ConstString& _fileSystemName ) = 0;
		virtual FileOutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const String& _filename ) = 0;
	};

}

bool initInterfaceSystem( Menge::FileSystemInterface**	_ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface* _ptrFileSystem );
