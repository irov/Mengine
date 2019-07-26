#include "PythonFramework.h"

#include "Interface/ScriptServiceInterface.h"
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
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Framework, Mengine::PythonFramework );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::PythonFramework()
        : m_initializeFramework( false )
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
        if( m_initializeFramework == true )
        {
            return true;
        }

        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ), GameScriptEmbedding );

        m_initializeFramework = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::onFrameworkFinalize()
    {
        if( m_initializeFramework == false )
        {
            return;
        }

        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ) );

        m_initializeFramework = false;
    }
}
