#include "Crimsonland3DGamePlugin.h"

#include "Interface/GameServiceInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Crimsonland3DGame, Mengine::Crimsonland3DGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DGamePlugin::Crimsonland3DGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DGamePlugin::~Crimsonland3DGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DGamePlugin::_initializePlugin()
    {
        m_game = Helper::makeFactorableUnique<Crimsonland3DGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "Crimsonland3DGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
