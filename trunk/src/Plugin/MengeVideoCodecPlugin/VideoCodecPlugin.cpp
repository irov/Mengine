#	include "VideoCodecPlugin.h"

//#   include "VideoDecoderGVF.h"
//#   include "VideoDecoderVPX.h"
#	include "VideoDecoderTheora.h"

#   include "Codec/DecoderFactory.h"

#   include "Interface/StringizeInterface.h"

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::VideoCodecPlugin();

		return true;
	}
#   ifdef MENGE_PLUGIN_DLL
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginMengeVideoCodec( _plugin );
	}
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
		        
		//m_decoders.push_back( new DecoderFactory<VideoDecoderGVF>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "gvfVideo")) );
        //m_decoders.push_back( new DecoderFactory<VideoDecoderVPX>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "vpxVideo")) );
		m_decoders.push_back( new DecoderFactory<VideoDecoderTheora>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ogvVideo")) );
		m_decoders.push_back( new DecoderFactory<VideoDecoderTheora>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ogvaVideo")) );
		
		
		for( TVectorVideoDecoders::iterator
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
	void VideoCodecPlugin::finalize()
	{
		for( TVectorVideoDecoders::iterator
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
	void VideoCodecPlugin::destroy()
	{
		delete this;
	}
}