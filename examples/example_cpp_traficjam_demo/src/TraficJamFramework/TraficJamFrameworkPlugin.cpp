#include "TraficJamFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "TraficJamFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TraficJamFramework, Mengine::TraficJamFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TraficJamFrameworkPlugin::TraficJamFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TraficJamFrameworkPlugin::~TraficJamFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TraficJamFramework" ), Helper::makeFrameworkFactory<TraficJamFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TraficJamFramework" ) );
    }
}
