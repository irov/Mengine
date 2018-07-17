#	include "SDLFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "SDLFileGroupDirectory.h"

#   include "Kernel/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SDLFileGroup, Mengine::SDLFileGroupPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class SDLFileGroupDirectoryFactory
            : public Factory
        {
        public:
            SDLFileGroupDirectoryFactory( const FilePath & _relationPath )
                : Factory( "SDLFileGroupDirectory" )
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
        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" )
                , new Detail::SDLFileGroupDirectoryFactory( STRINGIZE_FILEPATH_LOCAL( "" ) )
            );

        WChar currentPathW[MENGINE_MAX_PATH];
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

        String utf8_currentPath;
        Helper::unicodeToUtf8( currentPathW, utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePath( utf8_currentPath );

        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" )
                , new Detail::SDLFileGroupDirectoryFactory( relationPath )
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalize()
    {
        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" ) );

        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir") );
    }
}
