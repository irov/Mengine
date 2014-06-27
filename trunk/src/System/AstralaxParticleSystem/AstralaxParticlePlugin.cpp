#	include "AstralaxParticlePlugin.h"

#	include "Logger/Logger.h"

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginAstralaxParticlePlugin( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::AstralaxParticlePlugin();

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginAstralaxParticlePlugin( _plugin );
	}
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

		if( particleSystem->initialize() == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
		{
			return false;
		}

		m_particleSystem = particleSystem;

		return true;
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
	}
}