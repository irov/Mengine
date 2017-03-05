#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"

#	ifdef MENGINE_SUPPORT_DECODER_WEBP
#		include "ImageDecoderWEBP.h"
#	endif

#	include "ImageDecoderDDS.h"
#	include "ImageEncoderDDS.h"

#	include "ImageDecoderPVRTC.h"
#	include "ImageDecoderETC1.h"

//#	include "ImageDecoderCRN.h"

#	include "ImageDecoderHTF.h"
#	include "ImageEncoderHTF.h"
#	include "ImageDecoderACF.h"
#	include "ImageEncoderACF.h"

#	include "ImageEncoderPNG.h"
#	include "ImageEncoderJPEG.h"

#   include "PickDecoderHIT.h"
#   include "PickEncoderHIT.h"

#	include "DataflowAEK.h"
#	include "DataflowMDL.h"

#   include "Codec/DecoderFactory.h"
#   include "Codec/EncoderFactory.h"
#   include "Codec/DataflowFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeImageCodec, Menge::ImageCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodecPlugin::ImageCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPlugin::_initialize()
	{
        Helper::registerDecoder<ImageDecoderPNG>( m_serviceProvider, "pngImage" );
        Helper::registerDecoder<ImageDecoderJPEG>( m_serviceProvider, "jpegImage" );
        
#	ifdef MENGINE_SUPPORT_DECODER_WEBP
        Helper::registerDecoder<ImageDecoderWEBP>( m_serviceProvider, "webpImage" );        
#	endif

        Helper::registerDecoder<ImageDecoderPVRTC>( m_serviceProvider, "pvrImage" );
        Helper::registerDecoder<ImageDecoderETC1>( m_serviceProvider, "etcImage" );
        Helper::registerDecoder<ImageDecoderDDS>( m_serviceProvider, "ddsImage" );

        Helper::registerDecoder<ImageDecoderHTF>( m_serviceProvider, "htfImage" );
        Helper::registerDecoder<ImageDecoderACF>( m_serviceProvider, "acfImage" );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "png", STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "PNG", STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "jpg", STRINGIZE_STRING_LOCAL(m_serviceProvider, "jpegImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "jpeg", STRINGIZE_STRING_LOCAL(m_serviceProvider, "jpegImage") );

#	ifdef MENGINE_SUPPORT_DECODER_WEBP
		CODEC_SERVICE( m_serviceProvider )
			->registerCodecExt( "webp", STRINGIZE_STRING_LOCAL( m_serviceProvider, "webpImage" ) );
#	endif

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "pvr", STRINGIZE_STRING_LOCAL(m_serviceProvider, "pvrImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "dds", STRINGIZE_STRING_LOCAL(m_serviceProvider, "ddsImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "etc", STRINGIZE_STRING_LOCAL(m_serviceProvider, "etcImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "crn", STRINGIZE_STRING_LOCAL(m_serviceProvider, "crnImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "htf", STRINGIZE_STRING_LOCAL(m_serviceProvider, "htfImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "acf", STRINGIZE_STRING_LOCAL(m_serviceProvider, "acfImage") );

        Helper::registerEncoder<ImageEncoderPNG>( m_serviceProvider, "pngImage" );
        Helper::registerEncoder<ImageEncoderJPEG>( m_serviceProvider, "jpegImage" );
        Helper::registerEncoder<ImageEncoderHTF>( m_serviceProvider, "htfImage" );
        Helper::registerEncoder<ImageEncoderACF>( m_serviceProvider, "acfImage" );

        Helper::registerEncoder<ImageEncoderDDS>( m_serviceProvider, "ddsImage" );

        Helper::registerDecoder<PickDecoderHIT>( m_serviceProvider, "hitPick" );
        Helper::registerEncoder<PickEncoderHIT>( m_serviceProvider, "hitPick" );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "hit", STRINGIZE_STRING_LOCAL(m_serviceProvider, "hitPick") );

		m_factoryAEK = new DataflowFactory<DataflowAEK>();
        m_factoryAEK->setServiceProvider( m_serviceProvider );
        m_factoryAEK->initialize();

		DataflowInterfacePtr aek = m_factoryAEK->createDataflow();

		DATA_SERVICE(m_serviceProvider)
			->registerDataflow( STRINGIZE_STRING_LOCAL(m_serviceProvider, "aekMovie"), aek );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "aek", STRINGIZE_STRING_LOCAL(m_serviceProvider, "aekMovie") );

		m_factoryMDL = new DataflowFactory<DataflowMDL>();
        m_factoryMDL->setServiceProvider( m_serviceProvider );
        m_factoryMDL->initialize();

		DataflowInterfacePtr mdl = m_factoryMDL->createDataflow();

		DATA_SERVICE(m_serviceProvider)
			->registerDataflow( STRINGIZE_STRING_LOCAL(m_serviceProvider, "mdzModel"), mdl );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "mdz", STRINGIZE_STRING_LOCAL(m_serviceProvider, "mdzModel") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( m_serviceProvider, "pngImage" );
        Helper::unregisterDecoder( m_serviceProvider, "jpegImage" );

#	ifdef MENGINE_SUPPORT_DECODER_WEBP
        Helper::unregisterDecoder( m_serviceProvider, "webpImage" );
#	endif

        Helper::unregisterDecoder( m_serviceProvider, "pvrImage" );
        Helper::unregisterDecoder( m_serviceProvider, "etcImage" );
        Helper::unregisterDecoder( m_serviceProvider, "ddsImage" );

        Helper::unregisterDecoder( m_serviceProvider, "htfImage" );
        Helper::unregisterDecoder( m_serviceProvider, "acfImage" );
        Helper::unregisterEncoder( m_serviceProvider, "pngImage" );
        Helper::unregisterEncoder( m_serviceProvider, "jpegImage" );
        Helper::unregisterEncoder( m_serviceProvider, "htfImage" );
        Helper::unregisterEncoder( m_serviceProvider, "acfImage" );

        Helper::unregisterEncoder( m_serviceProvider, "ddsImage" );

        Helper::unregisterDecoder( m_serviceProvider, "hitPick" );
        Helper::unregisterEncoder( m_serviceProvider, "hitPick" );

		DATA_SERVICE(m_serviceProvider)
			->unregisterDataflow( STRINGIZE_STRING_LOCAL(m_serviceProvider, "aekMovie") );

		DATA_SERVICE(m_serviceProvider)
			->unregisterDataflow( STRINGIZE_STRING_LOCAL(m_serviceProvider, "mdzModel") );
	}
}