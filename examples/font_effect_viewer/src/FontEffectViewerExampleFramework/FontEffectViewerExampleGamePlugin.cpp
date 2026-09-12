#include "FontEffectViewerExampleGamePlugin.h"

#include "FontEffectViewerExamplePreviewCanvas.h"

#include "Interface/EventationInterface.h"
#include "Interface/GameServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FontEffectViewerExampleGame, Mengine::FontEffectViewerExampleGamePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleGamePlugin::FontEffectViewerExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleGamePlugin::~FontEffectViewerExampleGamePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleGamePlugin::_initializePlugin()
    {
        if( Helper::addDefaultPrototype<FontEffectViewerExamplePreviewCanvas, 4>( Node::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        m_game = Helper::makeFactorableUnique<FontEffectViewerExampleGame>( MENGINE_DOCUMENT_FACTORABLE );
        m_game->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerExampleGame" ) );

        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->addEventReceiver( EVENT_GAME_INITIALIZE, m_game );
        gameEventation->addEventReceiver( EVENT_GAME_FINALIZE, m_game );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleGamePlugin::_finalizePlugin()
    {
        EventationInterface * gameEventation = GAME_SERVICE()
            ->getEventation();

        gameEventation->removeEventReceiver( EVENT_GAME_INITIALIZE );
        gameEventation->removeEventReceiver( EVENT_GAME_FINALIZE );

        m_game = nullptr;

        Helper::removePrototype<FontEffectViewerExamplePreviewCanvas>( Node::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
}
