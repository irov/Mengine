#include "PythonFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "PythonFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ScriptProviderService );
SERVICE_EXTERN( ScriptService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PythonFramework, Mengine::PythonFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::~PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFrameworkPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( ScriptProviderService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( ScriptService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PythonFramework" ), Helper::makeFrameworkFactory<PythonFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PythonFramework" ) );

        SERVICE_FINALIZE( ScriptService );
        SERVICE_FINALIZE( ScriptProviderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ScriptService );
        SERVICE_DESTROY( ScriptProviderService );
    }
    //////////////////////////////////////////////////////////////////////////
}
