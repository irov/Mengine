
#	include "VorbisCallback.h"
#	include "Interface/FileSystemInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
size_t s_readOgg( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
{
	std::istream *file = reinterpret_cast<std::istream*>(_datasource);
	file->read( (char *)_ptr, (std::streamsize)(_size * _nmemb) );
	return file->gcount();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int s_seekOgg( void *_datasource, ogg_int64_t _offset, int _whence )
{
	std::istream *file = reinterpret_cast<std::istream*>(_datasource);
	std::ios_base::seekdir dir;
	file->clear();

	switch ( _whence )
	{
	case SEEK_SET: dir = std::ios::beg; break;
	case SEEK_CUR: dir = std::ios::cur; break;
	case SEEK_END: dir = std::ios::end; break;
	default: return -1;
	}

	file->seekg( (std::streamoff)_offset, dir );
	return file->fail() ? -1 : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
long s_tellOgg( void *_datasource )
{
	std::istream *file = reinterpret_cast<std::istream*>(_datasource);
	return file->tellg();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int s_closeOgg( void *_datasource )
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////
size_t s_readOgg_( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
{
	Menge::DataStreamInterface* stream = static_cast<Menge::DataStreamInterface*>( _datasource );
	std::size_t count = stream->read( _ptr, _size * _nmemb );
	return count;
}
//////////////////////////////////////////////////////////////////////////
int s_seekOgg_( void *_datasource, ogg_int64_t _offset, int _whence )
{
	Menge::DataStreamInterface* stream = static_cast<Menge::DataStreamInterface*>( _datasource );
	ogg_int64_t pos = _offset;
	switch( _whence )
	{
	case SEEK_SET: 
		break;
	case SEEK_CUR:
		pos += stream->tell();
		break;
	case SEEK_END:
		pos += stream->size();
		break;
	}
	stream->seek( pos );
	return 0;
}
//////////////////////////////////////////////////////////////////////////
long s_tellOgg_( void *_datasource )
{
	Menge::DataStreamInterface* stream = static_cast<Menge::DataStreamInterface*>( _datasource );
	return stream->tell();
}
//////////////////////////////////////////////////////////////////////////
int s_closeOgg_( void *_datasource )
{
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// new 28.04
/*size_t ReadOgg2( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
{
	mstream* file = (mstream*)_datasource;
	size_t count_to_read = (_nmemb * _size + file->position < file->length) 
		? _nmemb : (file->length - file->position) / _size;

	memcpy(_ptr, (void*)(file->data + file->position), count_to_read * _size);
	file->position += count_to_read * _size;
	return count_to_read;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int SeekOgg2( void *_datasource, ogg_int64_t _offset, int _whence )
{
	mstream* file = (mstream*)_datasource;
	size_t newCurPosition;
	switch ( _whence )
	{
	case SEEK_SET: 
		newCurPosition = (size_t)_offset; 
		break;
	case SEEK_CUR: 
		newCurPosition = file->position + (size_t)_offset; 
		break;
	case SEEK_END: 
		newCurPosition = file->length; 
		break;
	default: 
		return -1;
	}

	if(newCurPosition < 0)
	{
		file->position = 0;
	}
	else if(newCurPosition > file->length)
	{
		file->position = file->length;
	}
	else
	{
		file->position = newCurPosition;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
long TellOgg2( void *_datasource )
{
	mstream* file = (mstream*)_datasource;
	return (long)file->position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CloseOgg2( void *_datasource )
{
	return 0;
}*/
