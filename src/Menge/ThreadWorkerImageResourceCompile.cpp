#	include "ThreadWorkerImageResourceCompile.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadWorkerImageResourceCompile::ThreadWorkerImageResourceCompile()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadWorkerImageResourceCompile::~ThreadWorkerImageResourceCompile()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadWorkerImageResourceCompile::initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _file, const ConstString & _codecType )
	{
		m_serviceProvider = _serviceProvider;

		InputStreamInterfacePtr stream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( _category, _file );

		MemoryInputPtr memory = new MemoryInput();

		size_t streamSize = stream->size();
		void * buffer = memory->newMemory( streamSize );
		stream->read( buffer, streamSize );
		
		m_imageDecoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( _codecType );
		
		if( m_imageDecoder == nullptr )
		{
			return false;
		}

		if( m_imageDecoder->initialize( memory ) == false )
		{
			return false;
		}

		const ImageCodecDataInfo* dataInfo = m_imageDecoder->getCodecDataInfo();

		size_t image_width = dataInfo->width;
		size_t image_height = dataInfo->height;
		size_t image_channels = dataInfo->channels;
		PixelFormat image_format = dataInfo->format;

		m_texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createTexture( image_width, image_height, image_channels, image_format );

		const Rect & rect = m_texture->getHWRect();

		m_textureBuffer = m_texture->lock( &m_texturePitch, rect, false );

		if( m_textureBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadWorkerImageResourceCompile::initialize Invalid lock");

			return false;
		}

		ImageCodecOptions options;

		const RenderImageInterfacePtr & image = m_texture->getImage();

		options.channels = image->getHWChannels();
		options.pitch = m_texturePitch;

		m_imageDecoder->setOptions( &options );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadWorkerImageResourceCompile::onWork( size_t _id )
	{
		(void)_id;

		const ImageCodecDataInfo * data = m_imageDecoder->getCodecDataInfo();

		unsigned int bufferSize = m_texturePitch * data->height;
		if( m_imageDecoder->decode( m_textureBuffer, bufferSize ) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadWorkerImageResourceCompile::onWork Invalid decode");

			m_invalid = true;
			
			return false;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadWorkerImageResourceCompile::onDone( size_t _id )
	{
		(void)_id;

		m_texture->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadWorkerImageResourceCompile::finalize()
	{
		m_imageDecoder = nullptr;
		m_texture = nullptr;
	}
}