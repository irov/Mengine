#	include "AstralaxParticlePlugin2.h"

#	include "Interface/ParticleSystemInterface.h"

#	include "Logger/Logger.h"

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginAstralaxParticlePlugin2( Menge::PluginInterface ** _plugin )
	{
		stdex_allocator_initialize();

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
SERVICE_EXTERN( ParticleSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticlePlugin2::AstralaxParticlePlugin2()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticlePlugin2::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;
		
		LOGGER_INFO(m_serviceProvider)( "Initializing Particle System 3D..." );

		SERVICE_CREATE( m_serviceProvider, ParticleSystem );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::finalize()
	{
		SERVICE_FINALIZE( m_serviceProvider, Menge::ParticleSystemInterface2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::destroy()
	{
		SERVICE_DESTROY( m_serviceProvider, Menge::ParticleSystemInterface2 );

		delete this;

		stdex_allocator_finalize();
	}
}