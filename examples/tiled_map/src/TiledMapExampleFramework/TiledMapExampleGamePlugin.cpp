#include "TiledMapExampleGamePlugin.h"

#include "Interface/EventationInterface.h"
#include "Interface/GameServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TiledMapExampleGame, Mengine::TiledMapExampleGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleGamePlugin::TiledMapExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleGamePlugin::~TiledMapExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<TiledMapExampleGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "TiledMapExampleGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
