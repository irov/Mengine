#include "MCPFrameCaptureCommand.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "MCPFrameCaptureOperation.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPFrameCaptureCommand::MCPFrameCaptureCommand( MCPCommandHostInterface * _host )
        : m_host( _host )
        , m_capturing( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPFrameCaptureCommand::~MCPFrameCaptureCommand()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureCommand::endCapture()
    {
        m_capturing = false;
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPFrameCaptureCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        if( m_capturing == true )
        {
            _response->errorMessage = "another frame capture is already running";

            return EMCPCommandStatus::FAILURE;
        }

        const String & mode = m_host->getMCPMode();

        if( mode == "headless_logic" )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "screenshots are unavailable in headless_logic mode";

            return EMCPCommandStatus::FAILURE;
        }

        bool includeAlpha = _request.params.get( "includeAlpha", false );

        const RenderTargetInterfacePtr & playerTarget = PLAYER_SERVICE()
            ->getRenderTarget();

        if( playerTarget != nullptr )
        {
            uint32_t playerWidth = playerTarget->getHWWidth();
            uint32_t playerHeight = playerTarget->getHWHeight();

            m_capturing = true;

            _response->operation = Helper::makeFactorableUnique<MCPFrameCaptureOperation>( MENGINE_DOCUMENT_FACTORABLE, this, m_host, playerTarget, false, playerWidth, playerHeight, includeAlpha );

            return EMCPCommandStatus::DEFERRED;
        }

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getCurrentWindowResolution();

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        if( width == 0 )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "the render window has no capture resolution";

            return EMCPCommandStatus::FAILURE;
        }

        if( height == 0 )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "the render window has no capture resolution";

            return EMCPCommandStatus::FAILURE;
        }

        RenderTargetInterfacePtr target = RENDER_SYSTEM()
            ->createRenderTargetTexture( width, height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        if( target == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "the active render backend cannot create a capture target";

            return EMCPCommandStatus::FAILURE;
        }

        PLAYER_SERVICE()
            ->setRenderTarget( target );

        m_capturing = true;

        _response->operation = Helper::makeFactorableUnique<MCPFrameCaptureOperation>( MENGINE_DOCUMENT_FACTORABLE, this, m_host, target, true, width, height, includeAlpha );

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
}
