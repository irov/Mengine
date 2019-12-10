#include "AstralaxParticlePlugin.h"

#include "AstralaxInterface.h"

#include "AstralaxParticleModule.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AstralaxService );
SERVICE_EXTERN( AstralaxSystem );
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
    bool AstralaxParticlePlugin::_initializePlugin()
    {
        LOGGER_INFO( "Initializing Astralax Particle System..." );

        SERVICE_CREATE( AstralaxService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( AstralaxSystem, MENGINE_DOCUMENT_FACTORABLE );

        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAstralaxParticle" )
            , Helper::makeModuleFactory<AstralaxParticleModule>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AstralaxService );
        SERVICE_FINALIZE( AstralaxSystem );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticlePlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AstralaxService );
        SERVICE_DESTROY( AstralaxSystem );
    }
}