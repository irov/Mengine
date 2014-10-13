#include "ImageDecoderETC1.h"
#include "Utils/Logger/Logger.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/RenderSystemInterface.h"

#define PNG_BYTES_TO_CHECK 8

#	include <string.h>

static uint16_t readBEUint16(const char* pIn) 
{
	return (pIn[0] << 8) | pIn[1];
}
static const uint16_t ETC1_PKM_FORMAT_OFFSET = 6;
static const uint16_t ETC1_PKM_ENCODED_WIDTH_OFFSET = 8;
static const uint16_t ETC1_PKM_ENCODED_HEIGHT_OFFSET = 10;
static const uint16_t ETC1_PKM_WIDTH_OFFSET = 12;
static const uint16_t ETC1_PKM_HEIGHT_OFFSET = 14;

//////////////////////////////////////////////////////////////////////////
ImageDecoderETC1::ImageDecoderETC1()
	: ImageDecoder()
	, m_needSeek(false)
{
}
//////////////////////////////////////////////////////////////////////////
ImageDecoderETC1::~ImageDecoderETC1()
{
}
//////////////////////////////////////////////////////////////////////////
size_t ImageDecoderETC1::decode( void * _buffer, size_t _bufferSize )
{
	if(m_needSeek)
	{
		m_stream->seek(sizeof(ETC1Header));
	}
	m_stream->read(_buffer, _bufferSize);
	m_needSeek = true;
	return _bufferSize;
}
//////////////////////////////////////////////////////////////////////////
bool ImageDecoderETC1::_prepareData()
{
	if(m_needSeek)
	{
		m_stream->seek(0);
	}

	m_stream->read(&m_etc1_ptr, sizeof(ETC1Header));
	m_needSeek = false;

	if(strcmp(m_etc1_ptr.tag, "PKM 10"))
	{
		LOGGER_ERROR(m_serviceProvider)("ImageDecoderETC1::initialize Bad or not ETC1 file" );
		return false;
	}
	char * buffer = reinterpret_cast<char *>(&m_etc1_ptr);
	m_etc1_ptr.format = readBEUint16(buffer + ETC1_PKM_FORMAT_OFFSET);
	m_etc1_ptr.texHeight = readBEUint16(buffer + ETC1_PKM_ENCODED_HEIGHT_OFFSET);
	m_etc1_ptr.texWidth = readBEUint16(buffer + ETC1_PKM_ENCODED_WIDTH_OFFSET);
	m_etc1_ptr.origHeight = readBEUint16(buffer + ETC1_PKM_HEIGHT_OFFSET);
	m_etc1_ptr.origWidth = readBEUint16(buffer + ETC1_PKM_WIDTH_OFFSET);

	bool isValid = m_etc1_ptr.texWidth >= m_etc1_ptr.origWidth 
		&& m_etc1_ptr.texWidth - m_etc1_ptr.origWidth < 4 
		&& m_etc1_ptr.texHeight >= m_etc1_ptr.origHeight 
		&& m_etc1_ptr.texHeight - m_etc1_ptr.origHeight < 4;

	m_dataInfo.channels = 3;
	m_dataInfo.depth = 1;
	//m_dataInfo.format = Menge::PF_ETC1;
	m_dataInfo.width = m_etc1_ptr.texWidth;
	m_dataInfo.height = m_etc1_ptr.texHeight;
	m_dataInfo.mipmaps = 1;

	return isValid;
}
