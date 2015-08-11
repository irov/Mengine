#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Interface/StringizeInterface.h"

#	include "Core/ConstString.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeXmlCodec( Menge::PluginInterface ** _plugin )
    {
		stdex_allocator_initialize();

        *_plugin = new Menge::XmlCodecPlugin();

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
#	ifdef MENGE_PLUGIN_DLL
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeXmlCodec( _plugin );
    }
#	endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
      	
		CODEC_SERVICE(m_serviceProvider)
            ->registerDecoder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "xml2bin"), new DecoderFactory<XmlToBinDecoder>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "bin")) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::finalize()
	{
		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "xml2bin") );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::destroy()
	{
		delete this;

		stdex_allocator_finalize();
	}
}