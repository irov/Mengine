#	include "OggVorbisPlugin.h"

#	include "SoundDecoderOGGVorbis.h"

#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeOggVorbis, Menge::OggVorbisPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OggVorbisPlugin::OggVorbisPlugin()       
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OggVorbisPlugin::_initialize()
	{
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
	void OggVorbisPlugin::_finalize()
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
}