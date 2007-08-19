#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include <fstream>

class FileData 
	: public FileDataInterface
{
public:
	FileData(size_t size);
	FileData(void *_data, size_t size);
	FileData(std::ifstream& _stream);
	~FileData();

public:
	size_t	read(void* _buffer, size_t _elemsize, size_t _count);
	size_t	read_chars(char* _buffer, size_t _count);
	size_t	read_floats(float* _buffer, size_t _count);
	size_t	read_ushorts(short* _buffer, size_t _count);
	size_t	read_ints(int* _buffer, size_t _count);
	size_t	read_longs(long* _buffer, size_t _count);
	bool	eof()const;
	void	setPos(size_t _pos);
    size_t	size()const;

    const char* getBuffer() const;
	const char* getPos() const;
private:
	char* m_data;
	char* m_pos;
	char* m_endData;
	size_t m_size;
};