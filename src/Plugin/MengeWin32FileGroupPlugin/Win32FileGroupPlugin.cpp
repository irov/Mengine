#	include "Win32FileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/PlatformInterface.h"

#   include "Factory/FactoryDefault.h"

#   include "Core/FilePath.h"

#	include "Win32FileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeWin32FileGroup, Menge::Win32FileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class Win32FileGroupDirectoryFactory
            : public Factory
        {
        public:
            Win32FileGroupDirectoryFactory( ServiceProviderInterface * _serviceProvider, const FilePath & _relationPath )
                : Factory( _serviceProvider, "Win32FileGroupDirectory" )
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
        FILE_SERVICE( m_serviceProvider )
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "global" )
                , new Detail::Win32FileGroupDirectoryFactory( m_serviceProvider, STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "" ) )
            );

        WChar currentPathW[MENGINE_MAX_PATH];
        PLATFORM_SERVICE( m_serviceProvider )
            ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

        String utf8_currentPath;
        Helper::unicodeToUtf8( m_serviceProvider, currentPathW, utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePath( m_serviceProvider, utf8_currentPath );

		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir")
                , new Detail::Win32FileGroupDirectoryFactory( m_serviceProvider, relationPath )
            );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::_finalize()
	{
        FILE_SERVICE( m_serviceProvider )
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "global" ) );

		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
}