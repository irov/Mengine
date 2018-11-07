#include "Win32FileGroupPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/FactoryDefault.h"

#include "Kernel/FilePath.h"

#include "Win32FileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32FileGroup, Mengine::Win32FileGroupPlugin )
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
    bool Win32FileGroupPlugin::_initialize()
    {
        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" )
                , new Detail::Win32FileGroupDirectoryFactory( STRINGIZE_FILEPATH_LOCAL( "" ) )
            );

        WChar currentPathW[MENGINE_MAX_PATH];
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

        String utf8_currentPath;
        Helper::unicodeToUtf8( currentPathW, utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePath( utf8_currentPath );

        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" )
                , new Detail::Win32FileGroupDirectoryFactory( relationPath )
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupPlugin::_finalize()
    {
        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" ) );

        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" ) );
    }
}