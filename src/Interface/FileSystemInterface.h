#	pragma once

#	include "Config/Typedef.h"

#	include <iosfwd>

namespace Menge
{
	class LogSystemInterface;

	class	DataStreamInterface
	{
	public:
		virtual void release() = 0;
		virtual std::streamsize read( void* _buf, std::streamsize _count ) = 0;
		//virtual String getLine( bool _trimAfter = true ) = 0;
		//virtual std::streamsize skipLine(  const String& _delim ) = 0;
		virtual void seek( std::streamoff _pos ) = 0;
		virtual void skip( std::streamoff _count ) = 0;
		virtual std::streampos tell() const = 0;
		virtual bool eof() const = 0;
		virtual std::streamsize size() const = 0;
		virtual void* getBuffer() = 0;
		virtual void setFreeOnClose( bool _free ) = 0;
	};

	class OutStreamInterface
	{
	public:
		virtual void write( const void* _data, std::streamsize _count ) = 0;
		virtual void write( const String& _str ) = 0;
		virtual void write( int _num ) = 0;
		virtual void flush() = 0;
	};

	class	FileSystemInterface
	{
	public:
		virtual bool inititalize( LogSystemInterface* _logSystemInterface ) = 0;

		virtual void loadPath( const String& _path ) = 0;
		virtual void loadPak( const String& _pak ) = 0;
		virtual void unloadPak( const String& _pak ) = 0;

		virtual bool existFile( const String& _filename ) = 0;
		virtual bool deleteFile( const String& _filename ) = 0;
		virtual DataStreamInterface* openFile( const String& _filename ) = 0;
		virtual DataStreamInterface* createMemoryFile( void* _data, std::streamsize _size, bool _freeOnClose ) = 0;
		virtual void closeStream( DataStreamInterface* _stream ) = 0;

		virtual OutStreamInterface* openOutStream( const String& _filename, bool _binary ) = 0;
		virtual void closeOutStream( OutStreamInterface* _stream ) = 0;

		virtual bool createFolder(  const String& _path  ) = 0;
		virtual bool deleteFolder(  const String& _path  ) = 0;

		virtual bool initAppDataPath( const String& _game ) = 0;
		virtual const String& getAppDataPath() = 0;
	};
}

bool initInterfaceSystem( Menge::FileSystemInterface**	_ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface* _ptrFileSystem );