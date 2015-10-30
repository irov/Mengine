#	include "PluginAmplifier.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/ServiceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "ResourcePlaylist.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeAmplifier( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::PluginAmplifier();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeAmplifier( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginAmplifier::PluginAmplifier()
		: m_serviceProvider(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginAmplifier::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;

		SERVICE_CREATE( m_serviceProvider, Amplifier, Menge::AmplifierInterface );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Resource"), STRINGIZE_STRING_LOCAL(m_serviceProvider, "ResourcePlaylist"), new ResourcePrototypeGenerator<ResourcePlaylist, 8>(m_serviceProvider) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::finalize()
	{
		SERVICE_FINALIZE( m_serviceProvider, Menge::AmplifierInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::destroy()
	{
		SERVICE_DESTROY( m_serviceProvider, Menge::AmplifierInterface );
		
		delete this;
	}
}