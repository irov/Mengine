#include "TrafficJamGamePlugin.h"

#include "Interface/GameServiceInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TrafficJamGame, Mengine::TrafficJamGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TrafficJamGamePlugin::TrafficJamGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficJamGamePlugin::~TrafficJamGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamGamePlugin::_initializePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()->getEventation();

        m_game = Helper::makeFactorableUnique<TrafficJamGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "TrafficJamGame" ) );

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;
    }
}
