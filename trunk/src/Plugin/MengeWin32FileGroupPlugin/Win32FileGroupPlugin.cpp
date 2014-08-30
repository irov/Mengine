#	include "Win32FileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Win32FileGroupDirectory.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeWin32FileGroup( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::Win32FileGroupPlugin();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeWin32FileGroup( _plugin );
    }
#   endif
}
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
			->registerFileGroupFactory( CONST_STRING_LOCAL(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<Win32FileGroupDirectory> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( CONST_STRING_LOCAL(m_serviceProvider, "dir") );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupPlugin::destroy()
	{
		delete this;
	}
}