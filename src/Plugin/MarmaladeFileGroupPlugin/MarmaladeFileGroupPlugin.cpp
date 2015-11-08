#	include "Win32FileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Win32FileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_DECLARE( MengeWin32FileGroup, Menge::Win32FileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupPlugin::Win32FileGroupPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;
	
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<Win32FileGroupDirectory> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::destroy()
	{
		delete this;
	}
}