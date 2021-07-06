#include "PuzzleFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "PuzzleFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PuzzleFramework, Mengine::PuzzleFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    PuzzleFrameworkPlugin::PuzzleFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PuzzleFrameworkPlugin::~PuzzleFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PuzzleFramework" ), Helper::makeFrameworkFactory<PuzzleFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PuzzleFramework" ) );
    }
}
