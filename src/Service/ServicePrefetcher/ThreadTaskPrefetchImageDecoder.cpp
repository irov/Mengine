#	include "ThreadTaskPrefetchImageDecoder.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/CacheInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Factory/FactorableUnique.h"
#	include "Core/MemoryCacheInput.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetchImageDecoder::ThreadTaskPrefetchImageDecoder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchImageDecoder::setImageCodec( const ConstString & _codec )
	{
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	const ImageDecoderInterfacePtr & ThreadTaskPrefetchImageDecoder::getDecoder() const
	{
		return m_imageDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchImageDecoder::_onRun()
	{
		m_group = FILE_SERVICE(m_serviceProvider)
			->getFileGroup( m_pakName );

		if( m_group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't get group '%s'"
				, m_pakName.c_str()
				);

			return false;
		}

		m_stream = m_group->createInputFile( m_filePath, false );

		if( m_stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't create input file '%s'"
				, m_pakName.c_str()
				);

			return nullptr;
		}
		
		m_imageDecoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( m_codec );

		if( m_imageDecoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide create codec %s"
				, m_codec.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchImageDecoder::_onMain()
	{		
		if( m_group->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		size_t stream_size = m_stream->size();

		MemoryInputPtr memoryInput = CACHE_SERVICE( m_serviceProvider )
			->createMemoryInput();

		void * memory = memoryInput->newMemory( stream_size );

		if( memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: '%s' invalid alloc memory '%d'"
				, m_filePath.c_str() 
				, stream_size
				);

			return false;
		}

		if( m_stream->read( memory, stream_size ) != stream_size )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: '%s' invalid read stream '%d'"
				, m_filePath.c_str() 
				, stream_size
				);

			return false;
		}

		if( m_imageDecoder->prepareData( memoryInput ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: decoder for file '%s' was not initialize"
				, m_filePath.c_str() 
				);

			return false;
		}

		return true;
	}
}
