#include "XmlCodecPlugin.h"

#include "XmlToBinDecoder.h"

#include "Interface/StringizeInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( XmlCodec, Mengine::XmlCodecPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    XmlCodecPlugin::XmlCodecPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlCodecPlugin::_initialize()
    {
        Helper::registerDecoder<XmlToBinDecoder>( "xml2bin" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlCodecPlugin::_finalize()
    {
        Helper::unregisterDecoder( "xml2bin" );
    }
}
