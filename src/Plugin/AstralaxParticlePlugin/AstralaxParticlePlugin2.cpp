#include "AstralaxParticlePlugin2.h"

#include "Interface/ParticleSystemInterface.h"
#include "../Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "AstralaxParticleModule2.h"
#include "ParticleResourceValidateVisitor.h"

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

        VisitorPtr particleValidateVisitor = new FactorableUnique<ParticleResourceValidateVisitor>();

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( particleValidateVisitor );

        m_particleValidateVisitor = particleValidateVisitor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin2::_finalize()
    {
        SERVICE_FINALIZE( Mengine::ParticleSystemInterface2 );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( m_particleValidateVisitor );
        m_particleValidateVisitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin2::_destroy()
    {
        SERVICE_DESTROY( Mengine::ParticleSystemInterface2 );
    }
}