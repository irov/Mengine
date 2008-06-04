#	pragma once

#	include "Config/Typedef.h"

class	DataStreamInterface
{
public:
	virtual std::size_t read( void* _buf, std::size_t _count ) = 0;
	virtual Menge::String getLine( bool _trimAfter = true ) = 0;
	virtual std::size_t skipLine(  const Menge::String& _delim ) = 0;
	virtual void seek( std::size_t _pos ) = 0;
	virtual std::size_t tell() const = 0;
	virtual bool eof() const = 0;
	virtual std::size_t size() const = 0;
	virtual void* getBuffer() = 0;
	virtual bool isMemory() const = 0;
	virtual void setFreeOnClose( bool _free ) = 0;
};

class	FileSystemInterface
{
public:
	virtual void loadPath( const Menge::String& _path ) = 0;
	virtual void loadPak( const Menge::String& _pak ) = 0;
	virtual void unloadPak( const Menge::String& _pak ) = 0;

	virtual bool existFile( const Menge::String& _filename ) = 0;
	virtual DataStreamInterface* openFile( const Menge::String& _filename ) = 0;
	virtual DataStreamInterface* createMemoryFile( void* _data, std::size_t _size, bool _freeOnClose ) = 0;
	virtual void closeStream( DataStreamInterface* _stream ) = 0;

	virtual const char * platformBundlePath() = 0;

	virtual bool createFolder(  const Menge::String& _path  ) = 0;
	virtual bool deleteFolder(  const Menge::String& _path  ) = 0;
	virtual bool changeDir(  const Menge::String& _path  ) = 0;

	//virtual const TCHAR * getApplicationDataPath( const TCHAR* _game ) = 0;
};

bool initInterfaceSystem( FileSystemInterface**	_ptrFileSystem );
void releaseInterfaceSystem( FileSystemInterface* _ptrFileSystem );