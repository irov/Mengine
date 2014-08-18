#	include "SoundCodecPlugin.h"

#	include "SoundDecoderOGGVorbis.h"
#	include "SoundDecoderWAV.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"


extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::SoundCodecPlugin();

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
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundCodecPlugin::SoundCodecPlugin()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;

		m_decoders.push_back( new DecoderFactory<SoundDecoderOGGVorbis>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "oggSound")) );
        m_decoders.push_back( new DecoderFactory<SoundDecoderOGGVorbis>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ogvSound")) );
		m_decoders.push_back( new DecoderFactory<SoundDecoderWAV>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "wavSound")) );
		
		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );

		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "wav", Helper::stringizeString(m_serviceProvider, "wavSound") );

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

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::destroy()
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

		delete this;
	}
}