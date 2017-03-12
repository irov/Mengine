#	include "AstralaxParticlePlugin2.h"

#	include "Interface/ParticleSystemInterface.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ParticleSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AstralaxParticlePlugin2, Menge::AstralaxParticlePlugin2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticlePlugin2::AstralaxParticlePlugin2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticlePlugin2::_initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Particle System 3D..." );

		SERVICE_CREATE( m_serviceProvider, ParticleSystem );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::_finalize()
	{
		SERVICE_FINALIZE( m_serviceProvider, Menge::ParticleSystemInterface2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::_destroy()
	{
		SERVICE_DESTROY( m_serviceProvider, Menge::ParticleSystemInterface2 );
	}
}