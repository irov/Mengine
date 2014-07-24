#	include "LZ4Plugin.h"

#   include "Interface/StringizeInterface.h"

#	include "ArchivatorLZ4.h"

#	include "Factory/FactorableUnique.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeLZ4( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::LZ4Plugin();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeLZ4( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LZ4Plugin::LZ4Plugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool LZ4Plugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;
	
		ARCHIVE_SERVICE(m_serviceProvider)
			->registerArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4"), new FactorableUnique<ArchivatorLZ4>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LZ4Plugin::destroy()
	{
		ARCHIVE_SERVICE(m_serviceProvider)
			->unregisterArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4") );

		delete this;
	}
}