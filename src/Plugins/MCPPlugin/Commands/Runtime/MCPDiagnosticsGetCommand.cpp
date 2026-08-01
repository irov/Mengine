#include "MCPDiagnosticsGetCommand.h"

#include "../../Contexts/MCPResourceContext.h"
#include "../../Contexts/MCPRuntimeContext.h"
#include "../../Contexts/MCPSceneContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/TimelineServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDiagnosticsGetCommand::MCPDiagnosticsGetCommand( MCPCommandHostInterface * _host, MCPRuntimeContext * _runtimeContext, MCPSceneContext * _sceneContext, MCPResourceContext * _resourceContext, MCPScriptContext * _scriptContext )
        : m_host( _host )
        , m_runtimeContext( _runtimeContext )
        , m_sceneContext( _sceneContext )
        , m_resourceContext( _resourceContext )
        , m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDiagnosticsGetCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        m_sceneContext->refreshGeneration();

        const String & mode = m_host->getMCPMode();
        bool connected = m_host->isMCPConnected();
        bool debuggerPaused = m_host->isMCPDebuggerPaused();
        uint32_t sceneGeneration = m_sceneContext->getGeneration();
        uint64_t updateGenerationValue = m_host->getMCPUpdateGeneration();
        int64_t updateGeneration = (int64_t)updateGenerationValue;
        bool updateFrozen = APPLICATION_SERVICE()->isUpdateFrozen();
        bool renderFrozen = APPLICATION_SERVICE()->isRenderFrozen();
        bool runtimePaused = m_runtimeContext->isPaused();
        float timeScale = TIMELINE_SERVICE()->calcTimeFactor();
        ConstString platform = RENDER_SYSTEM()->getRenderPlatformName();
        bool desktop = PLATFORM_SERVICE()->isDesktop();
        size_t overlayCountValue = m_resourceContext->getOverlayGroupCount();
        uint32_t overlayCount = (uint32_t)overlayCountValue;
        size_t scriptHandleCountValue = m_scriptContext->getHandleCount();
        uint32_t scriptHandleCount = (uint32_t)scriptHandleCountValue;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        bool scriptEmbedded = true;
#else
        bool scriptEmbedded = false;
#endif

        _response->result.set( "protocol", 1 );
        _response->result.set( "mode", mode );
        _response->result.set( "connected", connected );
        _response->result.set( "debuggerPaused", debuggerPaused );
        _response->result.set( "sceneGeneration", sceneGeneration );
        _response->result.set( "updateGeneration", updateGeneration );
        _response->result.set( "updateFrozen", updateFrozen );
        _response->result.set( "renderFrozen", renderFrozen );
        _response->result.set( "runtimePaused", runtimePaused );
        _response->result.set( "timeScale", timeScale );
        _response->result.set( "platform", platform );
        _response->result.set( "desktop", desktop );
        _response->result.set( "resourceOverlayGroups", overlayCount );
        _response->result.set( "scriptHandles", scriptHandleCount );
        _response->result.set( "scriptEmbedded", scriptEmbedded );

        const Resolution & resolution = APPLICATION_SERVICE()->getCurrentWindowResolution();
        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();
        bool fullscreen = APPLICATION_SERVICE()->getFullscreenMode();
        bool focus = APPLICATION_SERVICE()->isFocus();
        jpp::object window = jpp::make_object();
        window.set( "width", width );
        window.set( "height", height );
        window.set( "fullscreen", fullscreen );
        window.set( "focus", focus );
        _response->result.set( "window", window );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
