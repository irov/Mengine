#include "Win32FileGroupPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

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
                Win32FileGroupDirectory * t = Helper::newT<Win32FileGroupDirectory>();

                t->setRelationPath( m_relationPath );

                return t;
            }

            void _destroyObject( Factorable * _obj ) override
            {
                Helper::deleteT( _obj );
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
        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), Helper::makeFactory<Detail::Win32FileGroupDirectoryFactory>( STRINGIZE_FILEPATH_LOCAL( "" ) ) );

        Char currentPath[MENGINE_MAX_PATH];
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathLen );

        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), Helper::makeFactory<Detail::Win32FileGroupDirectoryFactory>( relationPath ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
}