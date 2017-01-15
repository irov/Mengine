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
        Helper::registerDecoder<XmlToBinDecoder>( m_serviceProvider, "xml2bin" );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( m_serviceProvider, "xml2bin" );
	}
}
