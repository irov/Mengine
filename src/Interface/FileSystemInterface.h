#pragma once

class	FileDataInterface
{
public:
    virtual const char*	getBuffer() const = 0;
	virtual const char*	getPos() const = 0;
	virtual bool	eof() const = 0;
	virtual size_t	size() const = 0;
	virtual size_t  read(void* _buffer, size_t _elemsize, size_t _count) = 0;
	virtual size_t	read_chars(char* _buffer, size_t _count) = 0;
	virtual size_t	read_floats(float* _buffer, size_t _count) = 0;
	virtual size_t	read_ushorts(short* _buffer, size_t _count) = 0;
	virtual size_t	read_ints(int* _buffer, size_t _count) = 0;
	virtual size_t	read_longs(long* _buffer, size_t _count) = 0;
	virtual	void	setPos(size_t _pos) = 0;
};

class	FileSystemInterface
{
public:
	virtual bool	loadPak(const char * _filename, int _prior) = 0;
	virtual bool	unloadPak(const char * _filename) = 0;
 	virtual bool	existFile(const char * _filename) = 0;
	virtual bool	createFolder(const char * _foldername) = 0;
	virtual FileDataInterface*	openFile(const char *	_filename) = 0;
	virtual void	closeFile(FileDataInterface* _fd) = 0;
};

bool initInterfaceSystem(FileSystemInterface**	_ptrFileSystem);
void releaseInterfaceSystem(FileSystemInterface*	_ptrFileSystem);