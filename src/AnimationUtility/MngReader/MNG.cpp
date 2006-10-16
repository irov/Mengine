#	include "MNG.h"
#	include <assert.h>

namespace mnglib
{

	void	readChunk(void* _buffer, size_t _elemsize, size_t _count, unsigned char*& _chunk)
	{
		size_t count = _elemsize * _count;
		memcpy(_buffer, (const void*)_chunk, count);
		_chunk += count;
	}

	void	readMNG(mngDesc& _desc, void* _buffer, int _size)
	{
		unsigned char*	pos = reinterpret_cast<unsigned char*>(_buffer);

		readChunk(&_desc.width, sizeof(unsigned int), 1, pos);
		readChunk(&_desc.height, sizeof(unsigned int), 1, pos);

		unsigned int	numSprites;
		readChunk(&numSprites, sizeof(unsigned int), 1, pos);
		_desc.frames.resize(numSprites);

		for (unsigned int i = 0; i < numSprites; i++)
		{
			readChunk(&_desc.frames[i].index, sizeof(unsigned int), 1, pos);
			readChunk(&_desc.frames[i].delay, sizeof(unsigned int), 1, pos);
		}

		unsigned int	numImages;
		readChunk(&numImages, sizeof(unsigned int), 1, pos);
		_desc.images.resize(numImages);

		for (unsigned int i = 0; i < numImages; i++)
		{
			readChunk(&_desc.images[i].offsetX, sizeof(unsigned int), 1, pos);
			readChunk(&_desc.images[i].offsetY, sizeof(unsigned int), 1, pos);

			unsigned int sizeBuffer = 0;
			readChunk(&sizeBuffer, sizeof(unsigned int), 1, pos);
			_desc.images[i].size = sizeBuffer;

			_desc.images[i].buffer = new unsigned char[sizeBuffer];
			readChunk(_desc.images[i].buffer, sizeof(char), sizeBuffer,pos);
		}
	};

	void	freeMNG(mngDesc& _desc)
	{
		_desc.width = -1;
		_desc.height = -1;
		for(std::vector<Image>::iterator it = _desc.images.begin(); it != _desc.images.end(); ++it)
		{
			delete (*it).buffer;
		}
		_desc.images.clear();
		_desc.frames.clear();
	}
}; 