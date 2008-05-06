#	pragma once

#	include "Config/Config.h"

#	if TARGET_PLATFORM == TP_WINDOWS
#		include <windows.h>
#	endif

#	include <string>

class	FileDataInterface
{
public:
	virtual unsigned int read( void* _buffer, unsigned int _count ) = 0;
	virtual bool eof() const = 0;

	virtual unsigned int size() const = 0;

	virtual void seek( unsigned int _pos ) = 0;
	virtual unsigned int tell() const = 0;

	virtual std::string getLine( bool _trimAfter ) const = 0;
	virtual unsigned int skipLine( const std::string& _delim ) = 0;
};

class	FileSystemInterface
{
public:
	virtual void loadPath( const char * _path ) = 0;
	virtual void loadPak(const char * _filename ) = 0;
	virtual void unloadPak(const char * _filename) = 0;

	virtual bool existFile(const char * _filename) = 0;
	virtual FileDataInterface *	openFile(const char *	_filename) = 0;
	virtual void closeFile(FileDataInterface* _fd) = 0;

	virtual const char * platformBundlePath() = 0;

	virtual bool createFolder( const char * _path ) = 0;
	virtual bool deleteFolder( const char * _path ) = 0;
	virtual bool changeDir( const char* _path ) = 0;
	virtual void addResourceLocation( const char* _path ) = 0;
	virtual void initResources() = 0;

	virtual const TCHAR * getApplicationDataPath( const TCHAR* _game ) = 0;
};

bool initInterfaceSystem(FileSystemInterface**	_ptrFileSystem);
void releaseInterfaceSystem(FileSystemInterface* _ptrFileSystem);