#pragma once

#include <string>

class	FileDataInterface
{
public:
    virtual const	unsigned char*	getBuffer() const = 0;
	virtual bool	eof() const = 0;
	virtual size_t	size() const = 0;
	virtual size_t  read(void* _buffer, size_t _elemsize, size_t _count) = 0;
	virtual size_t	read_chars(unsigned char* _buffer, size_t _count) = 0;
	virtual size_t	read_floats(float* _buffer, size_t _count) = 0;
	virtual size_t	read_ushorts(unsigned short* _buffer, size_t _count) = 0;
	virtual size_t	read_ints(unsigned int* _buffer, size_t _count) = 0;
	virtual size_t	read_longs(unsigned long* _buffer, size_t _count) = 0;
	virtual	void	setPos(size_t _pos) = 0;
};

class	FileSystemInterface
{
public:
	virtual bool	loadPak(const std::string&	_filename, int _prior) = 0;
	virtual bool	unloadPak(const std::string& _filename) = 0;
 	virtual bool	existFile(const std::string& _filename) = 0;
	virtual bool	createFolder(const std::string&	_foldername) = 0;
	virtual FileDataInterface*	openFile(const std::string&	_filename) = 0;
	virtual void	closeFile(FileDataInterface* _fd) = 0;
};

bool initInterfaceSystem(FileSystemInterface**	_ptrFileSystem);
void releaseInterfaceSystem(FileSystemInterface*	_ptrFileSystem);