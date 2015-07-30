#	include "AstralaxParticlePlugin2.h"

#	include "Logger/Logger.h"

SERVICE_EXTERN(ParticleSystem2, Menge::ParticleSystemInterface2);

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginAstralaxParticlePlugin2( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::AstralaxParticlePlugin2();

		return true;
	}
#	ifdef MENGE_PLUGIN_DLL
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginAstralaxParticlePlugin2( _plugin );
	}
#	endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticlePlugin2::AstralaxParticlePlugin2()
		: m_serviceProvider(nullptr)
		, m_particleSystem2(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticlePlugin2::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;

		LOGGER_INFO(m_serviceProvider)( "Initializing Particle System 3D..." );

		ParticleSystemInterface2 * particleSystem2;
		if( SERVICE_CREATE( ParticleSystem2, &particleSystem2 ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticlePlugin2::initialize Failed to initialize ParticleSystem2"
				);

			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleSystem2 ) == false )
		{
			return false;
		}

		if( particleSystem2->initialize() == false )
		{
			return false;
		}

		m_particleSystem2 = particleSystem2;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::finalize()
	{
		if( m_particleSystem2 != nullptr )
		{
			m_particleSystem2->finalize();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::destroy()
	{
		if( m_particleSystem2 != nullptr )
		{            
			SERVICE_DESTROY( ParticleSystem2, m_particleSystem2 );
			m_particleSystem2 = nullptr;
		}

		delete this;
	}
}