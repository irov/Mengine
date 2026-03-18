#include "JewelryGamePlugin.h"

#include "Interface/GameServiceInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( JewelryGame, Mengine::JewelryGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    JewelryGamePlugin::JewelryGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryGamePlugin::~JewelryGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<JewelryGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "JewelryGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE, m_game );

        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
