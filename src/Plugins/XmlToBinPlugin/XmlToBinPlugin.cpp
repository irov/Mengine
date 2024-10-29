#include "XmlToBinPlugin.h"

#include "XmlToBinConverter.h"

#include "Kernel/ConstString.h"
#include "Kernel/DecoderFactory.h"
#include "Kernel/ConverterFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"

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
        Helper::registerConverter<XmlToBinConverter>( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinPlugin::_finalizePlugin()
    {
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "xml2bin" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
