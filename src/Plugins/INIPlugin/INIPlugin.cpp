#include "INIPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "INIConfig.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( INI, Mengine::INIPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    INIPlugin::INIPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    INIPlugin::~INIPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIPlugin::_initializePlugin()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Config" ), STRINGIZE_STRING_LOCAL( "ini" ), Helper::makeDefaultPrototypeGenerator<INIConfig, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void INIPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Config" ), STRINGIZE_STRING_LOCAL( "ini" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void INIPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}