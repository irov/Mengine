#include "GraphicsGamePlugin.h"

#include "Interface/GameServiceInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GraphicsGame, Mengine::GraphicsGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    GraphicsGamePlugin::GraphicsGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsGamePlugin::~GraphicsGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsGamePlugin::_initializePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()->getEventation();

        m_game = Helper::makeFactorableUnique<GraphicsGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "GraphicsGame" ) );

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;
    }
}
