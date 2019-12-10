#include "XmlToBinPlugin.h"

#include "XmlToBinDecoder.h"

#include "Kernel/ConstString.h"
#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

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
        Helper::registerDecoder<XmlToBinDecoder>( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "xml2bin" ) );
    }
}
