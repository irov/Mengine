#include "ThreadTaskPrefetchImageDecoder.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/StringizeInterface.h"

#include "Factory/FactorableUnique.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetchImageDecoder::ThreadTaskPrefetchImageDecoder()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchImageDecoder::~ThreadTaskPrefetchImageDecoder()
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
        if( ThreadTaskPrefetch::_onRun() == false )
        {
            return false;
        }

		m_fileGroup = FILE_SERVICE()
			->getFileGroup( m_fileGroup );

		if( m_fileGroup == nullptr )
		{
			LOGGER_ERROR("FileEngine::openInputFile can't get group '%s'"
				, m_fileGroup.c_str()
				);

			return false;
		}

		m_stream = m_fileGroup->createInputFile( m_filePath, false );

		if( m_stream == nullptr )
		{
			LOGGER_ERROR("FileEngine::openInputFile can't create input file '%s'"
				, m_fileGroup.c_str()
				);

			return false;
		}
		
		m_imageDecoder = CODEC_SERVICE()
			->createDecoderT<ImageDecoderInterfacePtr>( m_codec );

		if( m_imageDecoder == nullptr )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide create codec %s"
				, m_codec.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchImageDecoder::_onMain()
	{	
		if( m_fileGroup->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s:%s'"
				, m_fileGroup.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
			->createMemoryInput();

		if( memoryInput == nullptr )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onRun: '%s:%s' invalide create memory input"
				, m_fileGroup.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		size_t stream_size = m_stream->size();

		void * memory = memoryInput->newMemory( stream_size );

		if( memory == nullptr )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onMain: '%s:%s' invalid alloc memory '%d'"
				, m_fileGroup.c_str()
				, m_filePath.c_str() 
				, stream_size
				);

			return false;
		}

		if( m_stream->read( memory, stream_size ) != stream_size )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onMain: '%s:%s' invalid read stream '%d'"
				, m_fileGroup.c_str()
				, m_filePath.c_str() 
				, stream_size
				);

			return false;
		}

		if( m_imageDecoder->prepareData( memoryInput ) == false )
		{
			LOGGER_ERROR("ThreadTaskPrefetcherTextureDecoder::_onMain: decoder for file '%s:%s' was not initialize"
				, m_fileGroup.c_str()
				, m_filePath.c_str() 
				);

			return false;
		}

		return true;
	}
}
