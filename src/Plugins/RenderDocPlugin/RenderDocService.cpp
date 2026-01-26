#include "RenderDocService.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

#ifndef MENGINE_RENDERDOC_DLLNAME
#define MENGINE_RENDERDOC_DLLNAME L"renderdoc.dll"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderDocService, Mengine::RenderDocService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderDocService::RenderDocService()
        : m_renderDocModule( NULL )
        , m_renderDocAPI( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderDocService::~RenderDocService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderDocService::_initializeService()
    {
        HMODULE renderDocModule = ::LoadLibraryW( MENGINE_RENDERDOC_DLLNAME );
        
        if( renderDocModule == NULL )
        {
            LOGGER_ERROR( "invalid load module '%ls' error: %ls"
                , MENGINE_RENDERDOC_DLLNAME
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)::GetProcAddress( renderDocModule, "RENDERDOC_GetAPI" );

        if( RENDERDOC_GetAPI == NULL  )
        {
            LOGGER_ERROR( "invalid module '%ls' get API '%s' error: %ls"
                , MENGINE_RENDERDOC_DLLNAME
                , "RENDERDOC_GetAPI"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }
         
        RENDERDOC_API_1_6_0 * rdoc_api = nullptr;
        if( RENDERDOC_GetAPI( eRENDERDOC_API_Version_1_6_0, (void **)&rdoc_api ) != 1 )
        {
            LOGGER_ERROR( "invalid module '%ls' RENDERDOC_GetAPI failed [eRENDERDOC_API_Version_1_6_0]"
                , MENGINE_RENDERDOC_DLLNAME
            );

            return false;
        }

        m_renderDocModule = renderDocModule;
        m_renderDocAPI = rdoc_api;

        m_renderDocAPI->SetCaptureTitle( "Mengine" );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_SCENE_BEGIN, &RenderDocService::notifyRenderSceneBegin_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_SCENE_END, &RenderDocService::notifyRenderSceneEnd_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocService::_finalizeService()
    {
        ::FreeLibrary( m_renderDocModule );

        m_renderDocAPI = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocService::notifyRenderSceneBegin_( const RenderPipelineInterfacePtr & _pipeline )
    {
        MENGINE_UNUSED( _pipeline );

        if( m_renderDocAPI->IsRemoteAccessConnected() == 0 )
        {
            return;
        }

        m_renderDocAPI->StartFrameCapture( NULL, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocService::notifyRenderSceneEnd_( const RenderPipelineInterfacePtr & _pipeline )
    {
        MENGINE_UNUSED( _pipeline );

        if( m_renderDocAPI->IsRemoteAccessConnected() == 0 )
        {
            return;
        }

        m_renderDocAPI->EndFrameCapture( NULL, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
}
