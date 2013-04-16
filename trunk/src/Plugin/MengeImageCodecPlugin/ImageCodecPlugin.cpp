#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/LogSystemInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
//#	include "ImageDecoderDDS.h"

#	include "ImageEncoderPNG.h"
//#   include "ImageDecoderCombinerRGBAndAlpha.h"
//#	include "VideoDecoderFFMPEG.h"
//#	include "VideoDecoderOGGTheora.h"
//#	include "SoundDecoderOGGVorbis.h"

#   include "PickDecoderHIT.h"
#   include "PickEncoderHIT.h"

#   include "Codec/DecoderFactory.h"
#   include "Codec/EncoderFactory.h"

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
	bool ImageCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;

        m_decoders.push_back( new DecoderFactory<ImageDecoderPNG>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "pngImage")) );
		m_decoders.push_back( new DecoderFactory<ImageDecoderJPEG>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "jpegImage")) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>(ConstString("jpgImage"), logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderMNE>("mneImage", logService) );
		//m_decoders.push_back( new DecoderFactory<ImageDecoderDDS>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "ddsImage")) );
		//m_decoders.push_back( ne w Detail::ImageDecoderSystem<ImageDecoderCombinerRGBAndAlpha>("combinedImage", logService) );
		
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderFFMPEG>("ffmpegVideo", logService) );

		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("oggVideo", logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("ogvVideo", logService) );
		//m_decoders.push_back( new Detail::DecoderSystem<SoundDecoderOGGVorbis>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "oggSound")) );
		//m_decoders.push_back( new Detail::DecoderSystem<SoundDecoderOGGVorbis>(m_serviceProvider, Helper::StringizeString(m_serviceProvider, "ogvSound")) );

        m_decoders.push_back( new DecoderFactory<PickDecoderHIT>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "hitPick")) );

        m_encoders.push_back( new EncoderFactory<PickEncoderHIT>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "hitPick")) );     
		m_encoders.push_back( new EncoderFactory<ImageEncoderPNG>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "pngImage")) );

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

			(*it)->destroy();
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

			(*it)->destroy();
		}

		delete this;
	}
}