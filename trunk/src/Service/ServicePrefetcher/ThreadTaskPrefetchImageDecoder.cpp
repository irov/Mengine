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
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchImageDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchImageDecoder::initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
	{
		m_pakName = _pakName;
		m_fileName = _fileName;
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	const ImageDecoderInterfacePtr & ThreadTaskPrefetchImageDecoder::getDecoder() const
	{
		return m_imageDecoderMemory;
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

		m_stream = m_group->createInputFile( m_fileName );

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

		m_imageDecoderMemory = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderMemoryPtr>( Helper::stringizeStringSize( m_serviceProvider, "memoryImage" ) );
		
		if( m_imageDecoderMemory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide create memoryImage"
				);

			return false;
		}

		m_memoryInput = CACHE_SERVICE(m_serviceProvider)
			->createMemoryInput();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchImageDecoder::_onMain()
	{		
		if( m_group->openInputFile( m_fileName, m_stream, 0, 0 ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				);

			return false;
		}

		if( m_stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				);

			return false;
		}

		if( m_imageDecoder->initialize( m_stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: Image decoder for file '%s' was not initialize"
				, m_fileName.c_str() 
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = m_imageDecoder->getCodecDataInfo();

		size_t memoryUse = dataInfo->width * dataInfo->height * dataInfo->channels;
		void * memory = m_memoryInput->newMemory( memoryUse );

		if( memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: '%s' invalid alloc memory '%d'"
				, m_fileName.c_str() 
				, memoryUse
				);

			return false;
		}
				
		ImageCodecOptions options;
		options.channels = dataInfo->channels;
		options.pitch = dataInfo->width * dataInfo->channels;

		if( m_imageDecoder->setOptions( &options ) == false )
		{
			return false;
		}

		size_t decodeByte = m_imageDecoder->decode( memory, memoryUse );
		
		if( decodeByte != memoryUse )
		{
			return false;
		}

		m_imageDecoder->finalize();
		
		if( m_imageDecoderMemory->initialize( m_memoryInput ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createImageDecoder_: Memory decoder for file '%s' was not initialize"
				, m_fileName.c_str() 
				);

			return nullptr;
		}

		m_imageDecoderMemory->setCodecDataInfo( dataInfo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchImageDecoder::_onComplete( bool _successful )
	{
		(void) _successful;

		m_group = nullptr;
		m_stream = nullptr;
		m_imageDecoder = nullptr;		
		m_memoryInput = nullptr;
	}
}
