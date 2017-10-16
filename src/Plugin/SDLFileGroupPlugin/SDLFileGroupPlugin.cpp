#	include "SDLFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "SDLFileGroupDirectory.h"

#   include "Factory/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeSDLFileGroup, Menge::SDLFileGroupPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class SDLFileGroupDirectoryFactory
            : public Factory
        {
        public:
            SDLFileGroupDirectoryFactory( ServiceProviderInterface * _serviceProvider, const FilePath & _relationPath )
                : Factory( _serviceProvider, "SDLFileGroupDirectory" )
                , m_relationPath( _relationPath )
            {
            }

        protected:
            Factorable * _createObject() override
            {
                SDLFileGroupDirectory * t = Helper::allocateT<SDLFileGroupDirectory>();

                t->setRelationPath( m_relationPath );

                return t;
            }

            void _destroyObject( Factorable * _obj ) override
            {
                Helper::freeT( _obj );
            }

        protected:
            FilePath m_relationPath;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupPlugin::SDLFileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupPlugin::_initialize()
    {
        FILE_SERVICE( m_serviceProvider )
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "global" )
                , new Detail::SDLFileGroupDirectoryFactory( m_serviceProvider, STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "" ) )
            );

        WChar currentPathW[MENGINE_MAX_PATH];
        PLATFORM_SERVICE( m_serviceProvider )
            ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

        String utf8_currentPath;
        Helper::unicodeToUtf8( m_serviceProvider, currentPathW, utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePath( m_serviceProvider, utf8_currentPath );

        FILE_SERVICE( m_serviceProvider )
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" )
                , new Detail::SDLFileGroupDirectoryFactory( m_serviceProvider, relationPath )
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalize()
    {
        FILE_SERVICE( m_serviceProvider )
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "global" ) );

        FILE_SERVICE(m_serviceProvider)
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
    }
}
