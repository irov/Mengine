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
		virtual bool open( const WString& _filename ) = 0;
		virtual void close() = 0;
	};

	class MappedFileInputStreamInterface
		: public FileInputStreamInterface
	{
	public:
		virtual void * getMemory() const = 0;
	};
	
	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const WString& _filename ) = 0;
		virtual void close() = 0;
		virtual int tell() = 0;
	};
	
	class FileSystemInterface
	{
	public:
		virtual bool existFile( const WString& _filename ) const = 0;
		
		virtual MappedFileInputStreamInterface* openMappedInputStream( const WString& _filename ) = 0;
		virtual void closeMappedInputStream( FileInputStreamInterface* _stream ) = 0;

		virtual FileInputStreamInterface* createInputStream() = 0;
		virtual void closeInputStream( FileInputStreamInterface* _stream ) = 0;
		
        virtual FileOutputStreamInterface* createOutputStream() = 0;
		virtual void closeOutputStream( FileOutputStreamInterface* _stream ) = 0;
		
		virtual bool deleteFile( const WString& _filename ) = 0;
		virtual bool createFolder( const WString& _path ) = 0;
		virtual bool deleteFolder( const WString& _path ) = 0;
	};
	
	class FileServiceInterface 
		: public ServiceInterface
	{
	public:
		virtual bool mountFileSystem( const ConstString& _fileSystemName, const WString& _path, const ConstString & _type, bool _create ) = 0;
		virtual void unmountFileSystem( const ConstString& _fileSystemName ) = 0;

	/*public:
		virtual bool getFullPath( const ConstString& _fileSystemName, const ConstString & _path, String & _fullPath ) const  = 0;
		virtual bool getFileSystemPath( const ConstString& _fileSystemName, String & _fileSystemPath ) const = 0 ;
	*/
	public:
		virtual bool existFile( const ConstString& _fileSystemName, const WString& _filename ) const = 0;

		//virtual FileInputStreamInterface * createInputFile( const ConstString& _fileSystemName ) = 0;
		virtual FileInputStreamInterface * openInputFile( const ConstString& _fileSystemName, const WString& _filename ) = 0;

		//virtual FileOutputStreamInterface * createOutputFile( const ConstString& _fileSystemName ) = 0;
		virtual FileOutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const WString& _filename ) = 0;
	};

}

bool initInterfaceSystem( Menge::FileSystemInterface**	_ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface* _ptrFileSystem );
