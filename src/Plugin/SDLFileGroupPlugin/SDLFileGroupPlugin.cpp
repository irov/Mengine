#	include "SDLFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "SDLFileGroupDirectory.h"

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
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<SDLFileGroupDirectory> >() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalize()
    {
        FILE_SERVICE(m_serviceProvider)
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
    }
}
