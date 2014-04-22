#	include "ImageDecoderETZ1.h"

#	include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Config/Blobject.h"

#	include "Logger/Logger.h"

#	include <string.h>

namespace Menge
{
#pragma pack( push, 1 )
	typedef struct
	{
		char tag[6];	       // "PKM 10"
		uint16_t format;         // Format == number of mips (== zero)
		uint16_t texWidth;       // Texture dimensions, multiple of 4 (big-endian)
		uint16_t texHeight;
		uint16_t origWidth;      // Original dimensions (big-endian)
		uint16_t origHeight;
	} ETC1_HEADER;
#pragma pack(pop)
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderETZ1::ImageDecoderETZ1()
		: m_uncompress_size(0)
		, m_compress_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderETZ1::~ImageDecoderETZ1()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderETZ1::_initialize()
	{
		m_stream->read( &m_uncompress_size, sizeof(m_uncompress_size) );
		m_stream->read( &m_compress_size, sizeof(m_compress_size) );

		ETC1_HEADER header;
		m_stream->read( &header, sizeof(header) );

		if( strncmp( header.tag, "PKM 10", 6 ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderETC1::initialize header not ETC1 file"				
				);

			return false;
		}

		if( header.format == 0 && 
			header.texWidth >= header.origWidth && 
			header.texWidth - header.origWidth < 4 && 
			header.texHeight >= header.origHeight && 
			header.texHeight - header.origHeight < 4 )
		{
			return false;
		}

		m_dataInfo.channels = 3;
		m_dataInfo.depth = 1;
		m_dataInfo.format = Menge::PF_ETC1;
		m_dataInfo.width = header.texWidth;
		m_dataInfo.height = header.texHeight;
		m_dataInfo.mipmaps = 0;
		m_dataInfo.flags = 0;

		m_dataInfo.size = m_stream->size() - sizeof(ETC1_HEADER);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderETZ1::decode( void * _buffer, size_t _bufferSize )
	{
		(void)_bufferSize;

		size_t uncompress_size;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->decompress( CONST_STRING_LOCAL(zip), m_stream, m_compress_size, _buffer, _bufferSize, uncompress_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::decode uncompress failed"
				);

			return 0;
		}		

		return uncompress_size;
	}
}