#	include "Win32FileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Factory/FactoryDefault.h"

#	include "Win32FileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeWin32FileGroup, Menge::Win32FileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupPlugin::Win32FileGroupPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupPlugin::_initialize()
	{
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir"), new FactoryDefault<Win32FileGroupDirectory>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::_finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
}