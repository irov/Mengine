#	include "AstralaxParticlePlugin.h"

#	include "Logger/Logger.h"

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginAstralaxParticlePlugin( Menge::PluginInterface ** _plugin )
	{
		stdex_allocator_initialize();

		*_plugin = new Menge::AstralaxParticlePlugin();

		return true;
	}
#   ifdef MENGE_PLUGIN_DLL
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginAstralaxParticlePlugin( _plugin );
	}
#	endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticlePlugin::AstralaxParticlePlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticlePlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;

		LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );

		ParticleSystemInterface * particleSystem;
		if( SERVICE_CREATE( ParticleSystem, &particleSystem ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticlePlugin::initialize Failed to initialize ParticleSystem"
				);

			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
		{
			return false;
		}

		if( particleSystem->initialize() == false )
		{
			SERVICE_UNREGISTRY( m_serviceProvider, particleSystem );

			return false;
		}

		m_particleSystem = particleSystem;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin::finalize()
	{
		if( m_particleSystem != nullptr )
		{
			m_particleSystem->finalize();			
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin::destroy()
	{
		if( m_particleSystem != nullptr )
		{            
			SERVICE_DESTROY( ParticleSystem, m_particleSystem );
			m_particleSystem = nullptr;
		}

		delete this;

		stdex_allocator_finalize();
	}
}