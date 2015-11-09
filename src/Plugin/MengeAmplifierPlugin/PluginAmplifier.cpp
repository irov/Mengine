#	include "PluginAmplifier.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/ServiceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "ResourcePlaylist.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Amplifier );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeAmplifier, Menge::PluginAmplifier );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginAmplifier::PluginAmplifier()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginAmplifier::_initialize()
	{
		SERVICE_CREATE( m_serviceProvider, Amplifier );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Resource"), STRINGIZE_STRING_LOCAL(m_serviceProvider, "ResourcePlaylist"), new ResourcePrototypeGenerator<ResourcePlaylist, 8>(m_serviceProvider) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::_finalize()
	{
		SERVICE_FINALIZE( m_serviceProvider, Menge::AmplifierInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::_destroy()
	{
		SERVICE_DESTROY( m_serviceProvider, Menge::AmplifierInterface );
	}
}