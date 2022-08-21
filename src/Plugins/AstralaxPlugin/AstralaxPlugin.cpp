#include "AstralaxPlugin.h"

#include "AstralaxInterface.h"
#include "AstralaxIncluder.h"

#include "AstralaxModule.h"

#include "ParticleConverterPTCToPTZ.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConverterFactory.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AstralaxService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Astralax, Mengine::AstralaxPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxPlugin::AstralaxPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxPlugin::~AstralaxPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxPlugin::_initializePlugin()
    {
        LOGGER_INFO( "astralax", "Initializing Astralax System..." );

        const char * version = Magic_GetVersion();

        //MING64 use 3.55 version...
#if !defined(MENGINE_TOOLCHAIN_MINGW64) && !defined(MENGINE_PLATFORM_APPLE)
        if( MENGINE_STRSTR( version, MAGIC_API ) == nullptr )
        {
            LOGGER_ERROR( "Astralax Magic Particles Version: '%s' but need '%s'"
                , version
                , MAGIC_API
            );

            return false;
        }
#endif

        LOGGER_MESSAGE( "Astralax Magic Particles Version: %s"
            , version
        );

        if( SERVICE_CREATE( AstralaxService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAstralax" )
            , Helper::makeModuleFactory<AstralaxModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerConverter<ParticleConverterPTCToPTZ>( "ptc2ptz", MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxPlugin::_finalizePlugin()
    {
        Helper::unregisterConverter( "ptc2ptz" );

        SERVICE_FINALIZE( AstralaxService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AstralaxService );
    }
    //////////////////////////////////////////////////////////////////////////
}
