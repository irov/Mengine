#include "TicTacToeFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "TicTacToeFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TicTacToeFramework, Mengine::TicTacToeFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TicTacToeFrameworkPlugin::TicTacToeFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TicTacToeFrameworkPlugin::~TicTacToeFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TicTacToeFramework" ), Helper::makeFrameworkFactory<TicTacToeFramework>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TicTacToeFramework" ) );
    }
}
