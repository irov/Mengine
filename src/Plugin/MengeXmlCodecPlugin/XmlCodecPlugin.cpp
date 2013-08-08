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
		: m_serviceProvider(NULL)
		, m_xml2bin(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
      		
		m_xml2bin = new DecoderFactory<XmlToBinDecoder>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "bin"));

		CODEC_SERVICE(m_serviceProvider)
            ->registerDecoder( Helper::stringizeString(m_serviceProvider, "xml2bin"), m_xml2bin );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::destroy()
	{
		CODEC_SERVICE(m_serviceProvider)
            ->unregisterDecoder( Helper::stringizeString(m_serviceProvider, "xml2bin") );

        m_xml2bin->destroy();
        m_xml2bin = nullptr;

		delete this;
	}
}