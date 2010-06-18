#	pragma once

#	include "StreamInterface.h"
#	include "Manager/ConstManager.h"

namespace Menge
{
	class FileInputInterface
		: public InputStreamInterface
	{
	public:
		virtual bool open( const ConstString& _filename ) = 0;
		virtual void close() = 0;

		virtual int tell() = 0;
	};

	class FileOutputInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const ConstString& _filename ) = 0;
		virtual void close() = 0;
		virtual int tell() = 0;
	};

	class FileSystemInterface
	{
	public:
		virtual bool existFile( const String& _filename ) = 0;
		
		virtual InputStreamInterface* openInputStream( const String& _filename ) = 0;
		virtual void closeInputStream( InputStreamInterface* _stream ) = 0;
		
		virtual OutputStreamInterface* openOutputStream( const String& _filename ) = 0;
		virtual void closeOutputStream( OutputStreamInterface* _stream ) = 0;
		
		virtual void* openMappedFile( const String& _filename, int* _size ) = 0;
		virtual void closeMappedFile( void* _file ) = 0;

		virtual bool deleteFile( const String& _filename ) = 0;
		virtual bool createFolder( const String& _path ) = 0;
		virtual bool deleteFolder( const String& _path ) = 0;
	};
}

bool initInterfaceSystem( Menge::FileSystemInterface**	_ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface* _ptrFileSystem );
