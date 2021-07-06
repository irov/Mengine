#include "JewelryFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "JewelryFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( JewelryFramework, Mengine::JewelryFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    JewelryFrameworkPlugin::JewelryFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryFrameworkPlugin::~JewelryFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "JewelryFramework" ), Helper::makeFrameworkFactory<JewelryFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "JewelryFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
