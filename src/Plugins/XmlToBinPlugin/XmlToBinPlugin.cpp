#include "XmlToBinPlugin.h"

#include "Interface/StringizeServiceInterface.h"

#include "XmlToBinDecoder.h"

#include "Kernel/ConstString.h"
#include "Kernel/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( XmlToBin, Mengine::XmlToBinPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    XmlToBinPlugin::XmlToBinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinPlugin::~XmlToBinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinPlugin::_initializePlugin()
    {
        Helper::registerDecoder<XmlToBinDecoder>( STRINGIZE_STRING_LOCAL( "xml2bin" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "xml2bin" ) );
    }
}
