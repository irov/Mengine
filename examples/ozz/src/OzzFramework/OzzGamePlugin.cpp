#include "OzzGamePlugin.h"

#include "Interface/GameServiceInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OzzGame, Mengine::OzzGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    OzzGamePlugin::OzzGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzGamePlugin::~OzzGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<OzzGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "OzzGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE, m_game );

        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
