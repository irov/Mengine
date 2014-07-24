#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/LogSystemInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderWEBP.h"
//#	include "ImageDecoderDDZ.h"
#	include "ImageDecoderPVRTC.h"
#	include "ImageDecoderETC1.h"
//#	include "ImageDecoderPVRTZ.h"
//#	include "ImageDecoderETZ1.h"
#	include "ImageDecoderCRN.h"

#	include "ImageDecoderHTF.h"
#	include "ImageEncoderHTF.h"
#	include "ImageDecoderACF.h"
#	include "ImageEncoderACF.h"

#	include "ImageEncoderPNG.h"

//#   include "ImageDecoderCombinerRGBAndAlpha.h"
//#	include "VideoDecoderFFMPEG.h"
//#	include "VideoDecoderOGGTheora.h"
//#	include "SoundDecoderOGGVorbis.h"

#   include "PickDecoderHIT.h"
#   include "PickEncoderHIT.h"

#	include "DataflowAEK.h"
#	include "DataflowMDL.h"

#   include "Codec/DecoderFactory.h"
#   include "Codec/EncoderFactory.h"
#   include "Codec/DataflowFactory.h"

#	include "Core/File.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::ImageCodecPlugin();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeImageCodec( _plugin );
    }
#   endif
}
////////////////////////////////////////////////////////////////////////////
//__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
//{
//	return initPluginMengeImageCodec( _plugin );
//}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodecPlugin::ImageCodecPlugin()
		: m_serviceProvider(nullptr)
		, m_factoryAEK(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;

        m_decoders.push_back( new DecoderFactory<ImageDecoderPNG>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "pngImage")) );
		m_decoders.push_back( new DecoderFactory<ImageDecoderJPEG>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "jpegImage")) );
        //m_decoders.push_back( new DecoderFactory<ImageDecoderDDZ>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ddsImage")) );
        //m_decoders.push_back( new DecoderFactory<ImageDecoderDDZ>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ddzImage")) );

#	ifdef MENGINE_SUPPORT_DECODER_WEBP
        m_decoders.push_back( new DecoderFactory<ImageDecoderWEBP>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "webpImage")) );
#	endif
		//m_decoders.push_back( new DecoderFactory<ImageDecoderPVRTZ>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "pvrtzImage")) );
		//m_decoders.push_back( new DecoderFactory<ImageDecoderETZ1>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "etz1Image")) );
		m_decoders.push_back( new DecoderFactory<ImageDecoderPVRTC>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "pvrImage")) );
		m_decoders.push_back( new DecoderFactory<ImageDecoderETC1>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "etcImage")) );

#	ifdef MENGINE_SUPPORT_DECODER_CRN
		m_decoders.push_back( new DecoderFactory<ImageDecoderCRN>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "crnImage")) );
#	endif

		m_decoders.push_back( new DecoderFactory<ImageDecoderHTF>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "htfImage")) );
		m_decoders.push_back( new DecoderFactory<ImageDecoderACF>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "acfImage")) );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "png", CONST_STRING_LOCAL(m_serviceProvider, "pngImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "PNG", CONST_STRING_LOCAL(m_serviceProvider, "pngImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "jpg", CONST_STRING_LOCAL(m_serviceProvider, "jpegImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "jpeg", CONST_STRING_LOCAL(m_serviceProvider, "jpegImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "pvz", CONST_STRING_LOCAL(m_serviceProvider, "pvrtzImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "etz", CONST_STRING_LOCAL(m_serviceProvider, "etz1Image") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "pvr", CONST_STRING_LOCAL(m_serviceProvider, "pvrImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "etc", CONST_STRING_LOCAL(m_serviceProvider, "etcImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "crn", CONST_STRING_LOCAL(m_serviceProvider, "crnImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "htf", CONST_STRING_LOCAL(m_serviceProvider, "htfImage") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "acf", CONST_STRING_LOCAL(m_serviceProvider, "acfImage") );

		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>(ConstString("jpgImage"), logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderMNE>("mneImage", logService) );
		//m_decoders.push_back( new DecoderFactory<ImageDecoderDDS>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "ddsImage")) );
		//m_decoders.push_back( ne w Detail::ImageDecoderSystem<ImageDecoderCombinerRGBAndAlpha>("combinedImage", logService) );
		
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderFFMPEG>("ffmpegVideo", logService) );

		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("oggVideo", logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("ogvVideo", logService) );
		//m_decoders.push_back( new Detail::DecoderSystem<SoundDecoderOGGVorbis>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "oggSound")) );
		//m_decoders.push_back( new Detail::DecoderSystem<SoundDecoderOGGVorbis>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "ogvSound")) );

        m_decoders.push_back( new DecoderFactory<PickDecoderHIT>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "hitPick")) );

        m_encoders.push_back( new EncoderFactory<PickEncoderHIT>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "hitPick")) );     
		m_encoders.push_back( new EncoderFactory<ImageEncoderPNG>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "pngImage")) );
		m_encoders.push_back( new EncoderFactory<ImageEncoderHTF>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "htfImage")) );
		m_encoders.push_back( new EncoderFactory<ImageEncoderACF>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "acfImage")) );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "hit", CONST_STRING_LOCAL(m_serviceProvider, "hitPick") );

		//VideoDecoderOGGTheora::createCoefTables_();

		for( TVectorDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
                ->registerDecoder( name, (*it) );
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
                ->registerEncoder( name, (*it) );
		}

		m_factoryAEK = new DataflowFactory<DataflowAEK>(m_serviceProvider);
		DataflowInterfacePtr aek = m_factoryAEK->createDataflow();

		DATA_SERVICE(m_serviceProvider)
			->registerDataflow( CONST_STRING_LOCAL(m_serviceProvider, "aekMovie"), aek );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "aek", CONST_STRING_LOCAL(m_serviceProvider, "aekMovie") );

		m_factoryMDL = new DataflowFactory<DataflowMDL>(m_serviceProvider);
		DataflowInterfacePtr mdl = m_factoryMDL->createDataflow();

		DATA_SERVICE(m_serviceProvider)
			->registerDataflow( CONST_STRING_LOCAL(m_serviceProvider, "mdzModel"), mdl );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "mdz", CONST_STRING_LOCAL(m_serviceProvider, "mdzModel") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::destroy()
	{
		for( TVectorDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
                ->unregisterDecoder( name );
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)
                ->unregisterEncoder( name );
		}

		DATA_SERVICE(m_serviceProvider)
			->unregisterDataflow( CONST_STRING_LOCAL(m_serviceProvider, "aekMovie") );

		DATA_SERVICE(m_serviceProvider)
			->unregisterDataflow( CONST_STRING_LOCAL(m_serviceProvider, "mdzModel") );

		delete this;
	}
}