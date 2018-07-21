#include "AstralaxParticlePlugin2.h"

#include "Interface/ParticleSystemInterface.h"

#include "AstralaxParticleModule2.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ParticleSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AstralaxParticlePlugin2, Mengine::AstralaxParticlePlugin2 );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticlePlugin2::AstralaxParticlePlugin2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticlePlugin2::~AstralaxParticlePlugin2()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticlePlugin2::_initialize()
	{
		LOGGER_INFO( "Initializing Particle System 3D..." );

		SERVICE_CREATE( ParticleSystem );

        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAstralaxParticle" )
            , new ModuleFactory<AstralaxParticleModule2>() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::_finalize()
	{
		SERVICE_FINALIZE( Mengine::ParticleSystemInterface2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticlePlugin2::_destroy()
	{
		SERVICE_DESTROY( Mengine::ParticleSystemInterface2 );
	}
}