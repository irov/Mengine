#	include "SDLFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "SDLFileGroupDirectory.h"

#   include "Factory/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeSDLFileGroup, Menge::SDLFileGroupPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupPlugin::SDLFileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupPlugin::_initialize()
    {
        FILE_SERVICE(m_serviceProvider)
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ), new FactoryDefault<SDLFileGroupDirectory>( m_serviceProvider ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalize()
    {
        FILE_SERVICE(m_serviceProvider)
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
    }
}
