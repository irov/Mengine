#	include "SoundCodecPlugin.h"

#	include "SoundDecoderWAV.h"

#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeSoundCodec, Menge::SoundCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundCodecPlugin::SoundCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundCodecPlugin::_initialize()
	{
		m_decoders.push_back( new DecoderFactory<SoundDecoderWAV>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "wavSound")) );
		
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
	void SoundCodecPlugin::_finalize()
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