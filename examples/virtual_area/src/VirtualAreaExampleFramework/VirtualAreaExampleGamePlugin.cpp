#include "VirtualAreaExampleGamePlugin.h"

#include "Interface/EventationInterface.h"
#include "Interface/GameServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( VirtualAreaExampleGame, Mengine::VirtualAreaExampleGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleGamePlugin::VirtualAreaExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleGamePlugin::~VirtualAreaExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaExampleGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<VirtualAreaExampleGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaExampleGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
