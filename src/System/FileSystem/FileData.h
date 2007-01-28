#	pragma once

#	include "interfaces.h"
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
	size_t	read(void* buffer, size_t elemsize, size_t count);
	size_t	read_chars(char* buffer, size_t count);
	size_t	read_floats(float* buffer, size_t count);
	size_t	read_ushorts(short* buffer, size_t count);
	size_t	read_ints(int* buffer, size_t count);
	size_t	read_longs(long* buffer, size_t count);
	bool	eof()const;
	void	setPos(size_t pos);
    size_t	size()const;
    const char* getBuffer() const;

private:
	char* m_data;
	char* m_pos;
	char* m_endData;
	size_t m_size;
};