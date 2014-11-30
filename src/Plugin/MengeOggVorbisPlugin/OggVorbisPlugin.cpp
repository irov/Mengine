#	include "OggVorbisPlugin.h"

#	include "SoundDecoderOGGVorbis.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"


extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeOggVorbis( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::OggVorbisPlugin();

        return true;
    }

#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeOggVorbis( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OggVorbisPlugin::OggVorbisPlugin()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OggVorbisPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;

		m_decoders.push_back( new DecoderFactory<SoundDecoderOGGVorbis>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "oggSound")) );
        m_decoders.push_back( new DecoderFactory<SoundDecoderOGGVorbis>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ogvSound")) );
		
		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );

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
	void OggVorbisPlugin::finalize()
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

		m_decoders.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void OggVorbisPlugin::destroy()
	{
		delete this;
	}
}