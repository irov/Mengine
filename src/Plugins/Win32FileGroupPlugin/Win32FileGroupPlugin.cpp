#include "Win32FileGroupPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/FactoryDefault.h"

#include "Kernel/FilePath.h"

#include "Win32FileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY(Win32FileGroup, Mengine::Win32FileGroupPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class Win32FileGroupDirectoryFactory
            : public Factory
        {
        public:
            Win32FileGroupDirectoryFactory( const FilePath & _relationPath )
                : Factory( "Win32FileGroupDirectory" )
                , m_relationPath( _relationPath )
            {
            }

        protected:
            Factorable * _createObject() override
            {
                Win32FileGroupDirectory * t = Helper::allocateT<Win32FileGroupDirectory>();

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
    Win32FileGroupPlugin::Win32FileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupPlugin::~Win32FileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupPlugin::_initializePlugin()
    {
        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" )
                , Helper::makeFactory<Detail::Win32FileGroupDirectoryFactory>( STRINGIZE_FILEPATH_LOCAL( "" ) )
            );

        Char currentPath[MENGINE_MAX_PATH];
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePath( currentPath );

        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" )
                , Helper::makeFactory<Detail::Win32FileGroupDirectoryFactory>( relationPath )
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupPlugin::_finalizePlugin()
    {
        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" ) );

        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" ) );
    }
}