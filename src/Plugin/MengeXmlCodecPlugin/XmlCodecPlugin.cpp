#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Interface/StringizeInterface.h"

#	include "Core/ConstString.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeXmlCodec, Menge::XmlCodecPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlCodecPlugin::_initialize()
	{ 	
		CODEC_SERVICE(m_serviceProvider)
            ->registerDecoder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "xml2bin"), new DecoderFactory<XmlToBinDecoder>(m_serviceProvider, STRINGIZE_STRING_LOCAL(m_serviceProvider, "bin")) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::_finalize()
	{
		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "xml2bin") );
	}
}
