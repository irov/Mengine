#include "AstralaxParticlePlugin.h"

#include "AstralaxInterface.h"

#include "Interface/StringizeInterface.h"

#include "Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "AstralaxParticleModule.h"
#include "ParticleResourceValidateVisitor.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AstralaxService );
SERVICE_EXTERN( ParticleSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AstralaxParticlePlugin, Mengine::AstralaxParticlePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticlePlugin::AstralaxParticlePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticlePlugin::~AstralaxParticlePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxParticlePlugin::_initialize()
    {
        LOGGER_INFO( "Initializing Astralax Particle System..." );
        
        SERVICE_CREATE( AstralaxService );
        SERVICE_CREATE( ParticleSystem );

        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAstralaxParticle" )
            , new ModuleFactory<AstralaxParticleModule>() );

        VisitorPtr particleValidateVisitor = new FactorableUnique<ParticleResourceValidateVisitor>();

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( particleValidateVisitor );

        m_particleValidateVisitor = particleValidateVisitor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin::_finalize()
    {
        SERVICE_FINALIZE( Mengine::AstralaxSystemInterface );
        SERVICE_FINALIZE( Mengine::AstralaxServiceInterface );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidateVisitor( m_particleValidateVisitor );
        m_particleValidateVisitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::AstralaxSystemInterface );
        SERVICE_DESTROY( Mengine::AstralaxServiceInterface );
    }
}