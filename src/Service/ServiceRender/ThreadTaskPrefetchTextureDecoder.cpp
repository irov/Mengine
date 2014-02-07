#	include "ThreadTaskPrefetchTextureDecoder.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/MemoryInput.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetchTextureDecoder::ThreadTaskPrefetchTextureDecoder( ServiceProviderInterface * _serviceProvider, const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec, RenderTextureDecoderReceiverInterface * _receiver )
		: m_serviceProvider(_serviceProvider)
		, m_pakName(_pakName)
		, m_fileName(_fileName)
		, m_codec(_codec)
		, m_receiver(_receiver)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchTextureDecoder::_onRun()
	{
		m_stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( m_pakName, m_fileName );

		if( m_stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				);

			return false;
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
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchTextureDecoder::_onMain()
	{		
		if( m_imageDecoder->initialize( m_stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onMain: Image decoder for file '%s' was not initialize"
				, m_fileName.c_str() 
				);

			return nullptr;
		}

		const ImageCodecDataInfo * dataInfo = m_imageDecoder->getCodecDataInfo();

		size_t memoryUse = dataInfo->width * dataInfo->height * dataInfo->channels;

		MemoryInputPtr memoryStream = new MemoryInput();

		void * buffer = memoryStream->newMemory( memoryUse );

		size_t decodeByte = m_imageDecoder->decode( buffer, memoryUse );

		if( decodeByte != memoryUse )
		{
			return false;
		}

		if( m_imageDecoderMemory->initialize( memoryStream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createImageDecoder_: Memory decoder for file '%s' was not initialize"
				, m_fileName.c_str() 
				);

			return nullptr;
		}

		m_imageDecoderMemory->setImageCodecDataInfo( *dataInfo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchTextureDecoder::_onComplete( bool _successful )
	{
		if( _successful == false )
		{
			return;
		}

		if( m_receiver != nullptr )
		{
			m_receiver->onReceiverTextureDecoder( m_imageDecoderMemory );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchTextureDecoder::destroy()
	{
		delete this;
	}
}
