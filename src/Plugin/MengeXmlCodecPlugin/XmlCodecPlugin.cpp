#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Interface/StringizeInterface.h"

#	include "Core/File.h"
#	include "Core/ConstString.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeXmlCodec( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::XmlCodecPlugin();

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
#ifdef MENGE_COMPILER_MSVC
	__declspec(dllexport) 
#endif
	bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeXmlCodec( _plugin );
    }
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
		: m_serviceProvider(nullptr)
		, m_xml2bin(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
      		
		m_xml2bin = new DecoderFactory<XmlToBinDecoder>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "bin"));

		CODEC_SERVICE(m_serviceProvider)
            ->registerDecoder( CONST_STRING_LOCAL(m_serviceProvider, "xml2bin"), m_xml2bin );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::destroy()
	{
		CODEC_SERVICE(m_serviceProvider)
            ->unregisterDecoder( CONST_STRING_LOCAL(m_serviceProvider, "xml2bin") );

        m_xml2bin = nullptr;

		delete this;
	}
}