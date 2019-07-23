#include "PythonFramework.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "ConstsScriptEmbedding.h"
#include "EngineScriptEmbedding.h"
#include "EntityScriptEmbedding.h"
#include "GameScriptEmbedding.h"
#include "HelperScriptEmbedding.h"
#include "MathScriptEmbedding.h"
#include "NodeScriptEmbedding.h"
#include "SoundScriptEmbedding.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Framework, Mengine::PythonFramework );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::~PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::_initializeService()
    {
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ), ConstsScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ), MathScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ), HelperScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ), NodeScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ), EntityScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ), SoundScriptEmbedding );
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ), EngineScriptEmbedding );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_finalizeService()
    {
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::onFrameworkInitialize()
    {
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ), GameScriptEmbedding );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::onFrameworkFinalize()
    {
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ) );
    }
}
