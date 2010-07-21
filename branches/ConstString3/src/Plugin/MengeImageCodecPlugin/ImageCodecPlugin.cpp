#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "VideoDecoderOGGTheora.h"
#	include "SoundDecoderOGGVorbis.h"

#	include "ImageEncoderPNG.h"

#	include "Utils/Core/File.h"

//////////////////////////////////////////////////////////////////////////
bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::ImageCodecPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releasePluginMengeImageCodec( Menge::PluginInterface *_plugin )
{
	delete static_cast<Menge::ImageCodecPlugin*>(_plugin);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		template<class T>
		class ImageDecoderSystem
			: public DecoderSystemInterface
		{
		public:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new T(_stream);
			}
		};

		template<class T>
		class ImageEncoderSystem
			: public EncoderSystemInterface
		{
		public:
			EncoderInterface * createEncoder( OutputStreamInterface * _stream ) override
			{
				return new T(_stream);
			}
		};
	}
	void ImageCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		ServiceInterface * service = _provider->getService( "Codec" );

		if( service == 0 )
		{
			return;
		}

		CodecServiceInterface * codecService = static_cast<CodecServiceInterface*>(service);

		codecService->registerDecoder( "pngImage", new Detail::ImageDecoderSystem<ImageDecoderPNG>() );

		codecService->registerDecoder( "pngImage", new Detail::ImageDecoderSystem<ImageDecoderPNG>() );
		codecService->registerDecoder( "jpegImage", new Detail::ImageDecoderSystem<ImageDecoderJPEG>() );
		codecService->registerDecoder( "jpgImage", new Detail::ImageDecoderSystem<ImageDecoderJPEG>() );
		codecService->registerDecoder( "mneImage", new Detail::ImageDecoderSystem<ImageDecoderMNE>() );

		codecService->registerDecoder( "oggVideo", new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>() );
		codecService->registerDecoder( "ogvVideo", new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>() );
		codecService->registerDecoder( "oggSound", new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>() );
		codecService->registerDecoder( "ogvSound", new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>() );

		// Encoders
		codecService->registerEncoder( "pngImage", new Detail::ImageEncoderSystem<ImageEncoderPNG>() );

		VideoDecoderOGGTheora::createCoefTables_();
	}
}