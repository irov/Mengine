#include "OzzFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "OzzFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OzzFramework, Mengine::OzzFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    OzzFrameworkPlugin::OzzFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzFrameworkPlugin::~OzzFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "OzzFramework" ), Helper::makeFrameworkFactory<OzzFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "OzzFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
