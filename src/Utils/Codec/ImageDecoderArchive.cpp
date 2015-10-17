#	include "ImageDecoderArchive.h"

#	include "Core/MemoryHelper.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderArchive::ImageDecoderArchive()
		: m_uncompressSize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderArchive::setArchivator( const ArchivatorInterfacePtr & _archivator )
	{
		m_archivator = _archivator;
	}
	//////////////////////////////////////////////////////////////////////////
	const ArchivatorInterfacePtr & ImageDecoderArchive::getArchivator() const
	{
		return m_archivator;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderArchive::setUncompressSize( size_t _size )
	{
		m_uncompressSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderArchive::getUncompressSize() const
	{
		return m_uncompressSize;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderArchive::_decode( void * _buffer, size_t _bufferSize )
	{
		if( _bufferSize < m_options.pitch * m_dataInfo.height )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderMemory::decode invalid bufferSize %d != (%d * %d)"
				, _bufferSize
				, m_options.pitch
				, m_dataInfo.height
				);

			return 0;
		}

		m_stream->seek( 0 );

		size_t read_byte = 0;
		
		if( m_dataInfo.channels == 4 && m_options.channels == 4 )
		{
			read_byte = this->decodeData_( _buffer, _bufferSize );
		}
		else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
		{
			read_byte = this->decodeData_( _buffer, _bufferSize );

			this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
		}
		else
		{
			return 0;
		}

		return read_byte;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderArchive::decodeData_( void * _buffer, size_t _bufferSize ) const
	{
		(void)_bufferSize;

		size_t decodyByte = 0;

		size_t stream_size;
		void * stream_memory;
		if( m_stream->memory( &stream_memory, &stream_size ) == false )
		{
			MemoryCacheBufferInterfacePtr buffer = Helper::createMemoryCacheStream( m_serviceProvider, m_stream, "ImageDecoderArchive::decodeData_" );

			if( buffer == nullptr )
			{
				return 0;
			}
						
			const void * cache_buffer = buffer->getMemory();
			size_t cache_size = buffer->getSize();

			decodyByte = this->decompressData_( cache_buffer, cache_size, _buffer, m_uncompressSize );
		}
		else
		{
			decodyByte = this->decompressData_( stream_memory, stream_size, _buffer, m_uncompressSize );
		}

		return decodyByte;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderArchive::decompressData_( const void * _source, size_t _sourceSize, void * _dest, size_t _capacityDest ) const
	{
		size_t read_byte = 0;

		if( m_options.pitch == m_dataInfo.width * m_dataInfo.channels )
		{
			size_t decompressSize;
			if( m_archivator->decompress( _dest, _capacityDest, _source, _sourceSize, decompressSize ) == false )
			{
				return 0;
			}

			read_byte = decompressSize;
		}
		else
		{
			MemoryCacheBufferInterfacePtr buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, _capacityDest, "ImageDecoderArchive::decompressData_" );

			if( buffer == nullptr )
			{
				return 0;
			}
			
			void * cache_buffer = buffer->getMemory();

			size_t decompressSize;
			if( m_archivator->decompress( cache_buffer, _capacityDest, _source, _sourceSize, decompressSize ) == false )
			{
				return 0;
			}

			unsigned char * buffer_ptr = static_cast<unsigned char *>(_dest);
			for( size_t j = 0; j != m_dataInfo.height; ++j )
			{
				read_byte += m_stream->read( buffer_ptr, m_dataInfo.width * m_dataInfo.channels );

				buffer_ptr += m_options.pitch;
			}
		}

		return read_byte;
	}
}
