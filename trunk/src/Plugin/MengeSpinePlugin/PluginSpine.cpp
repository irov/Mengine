#	include "PluginSpine.h"

#	include "Interface/StringizeInterface.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "Spine.h"
#	include "ResourceSpine.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeSpine( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::PluginSpine();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
		return initPluginMengeSpine( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginSpine::PluginSpine()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginSpine::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Spine" ), new NodePrototypeGenerator<Spine, 128>( m_serviceProvider ) );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Resource" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceSpine" ), new ResourcePrototypeGenerator<ResourceSpine, 128>( m_serviceProvider ) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginSpine::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginSpine::destroy()
	{
		delete this;
	}
}