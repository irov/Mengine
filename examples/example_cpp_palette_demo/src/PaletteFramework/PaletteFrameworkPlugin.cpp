#include "PaletteFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "PaletteFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PaletteFramework, Mengine::PaletteFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    PaletteFrameworkPlugin::PaletteFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PaletteFrameworkPlugin::~PaletteFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PaletteFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PaletteFramework" ), Helper::makeFrameworkFactory<PaletteFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PaletteFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PaletteFramework" ) );
    }
}
