#include "CastleRoyalFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "CastleRoyalFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( CastleRoyalFramework, Mengine::CastleRoyalFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    CastleRoyalFrameworkPlugin::CastleRoyalFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CastleRoyalFrameworkPlugin::~CastleRoyalFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CastleRoyalFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "CastleRoyalFramework" ), Helper::makeFrameworkFactory<CastleRoyalFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CastleRoyalFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "SimpleBox2DFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
