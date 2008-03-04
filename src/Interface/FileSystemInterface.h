#	pragma once

#	include <string>

class	FileDataInterface
{
public:
	virtual size_t read( void* _buffer, size_t _count ) = 0;
	virtual bool eof() const = 0;

	virtual size_t size() const = 0;

	virtual void seek( size_t _pos ) = 0;
	virtual size_t tell() const = 0;

	virtual std::string getLine( bool _trimAfter ) const = 0;
	virtual size_t skipLine( const std::string& _delim ) = 0;
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
};

bool initInterfaceSystem(FileSystemInterface**	_ptrFileSystem);
void releaseInterfaceSystem(FileSystemInterface* _ptrFileSystem);