#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"
#include "Interface/GameEventReceiverInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Eventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GameServiceInterface
        : public ServiceInterface
        , public Eventable
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "GameService" )

    public:
        virtual void run() = 0;
        virtual void stop() = 0;

    public:
        virtual void update() = 0;

    public:
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;

    public:
        virtual void setCursorMode( bool _mode ) = 0;

    public:
        virtual bool loadPersonality() = 0;
        virtual void removePersonality() = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

    public:
        virtual void setFocus( bool _focus ) = 0;
        virtual void setFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void setFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void setGameViewport( const Viewport & _viewport, float _aspect ) = 0;

        virtual bool close() = 0;

        virtual void userEvent( const ConstString & _event, const MapWParams & _params ) = 0;

        virtual void turnSound( bool _turn ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define GAME_SERVICE()\
    ((Mengine::GameServiceInterface*)SERVICE_GET(Mengine::GameServiceInterface))
//////////////////////////////////////////////////////////////////////////