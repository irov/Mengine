#include "Arena3DGamePlugin.h"

#include "Interface/EventationInterface.h"
#include "Interface/GameServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Arena3DGame, Mengine::Arena3DGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool Arena3DGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<Arena3DGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "Arena3DGame" ) );

        EventationInterface * eventation = GAME_SERVICE()->getEventation();
        eventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        eventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DGamePlugin::_finalizePlugin()
    {
        EventationInterface * eventation = GAME_SERVICE()->getEventation();
        eventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        eventation->removeEventReceiver( EVENT_GAME_FINALIZE );
        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
