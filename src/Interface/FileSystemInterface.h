#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class	InputStreamInterface
	{
	public:
		virtual int read( void* _buf, int _count ) = 0;
		virtual void seek( int _pos ) = 0;
		virtual int size() const = 0;
	};

	class OutputStreamInterface
	{
	public:
		virtual void write( const void* _data, int _count ) = 0;
		virtual void flush() = 0;
	};

	class	FileSystemInterface
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
