#	include "PluginAmplifier.h"

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
SERVICE_EXTERN(Amplifier, Menge::AmplifierInterface);
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

		AmplifierInterface * amplifier;
		if( SERVICE_CREATE( Amplifier, &amplifier ) == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, amplifier ) == false )
		{
			return false;
		}

		m_amplifier = amplifier;

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Resource"), CONST_STRING_LOCAL(m_serviceProvider, "ResourcePlaylist"), new ResourcePrototypeGenerator<ResourcePlaylist, 8>(m_serviceProvider) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::destroy()
	{
		SERVICE_DESTROY( Amplifier, m_amplifier );

		delete this;
	}
}