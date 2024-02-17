#include "RenderDocService.h"

#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderDocService, Mengine::RenderDocService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderDocService::RenderDocService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderDocService::~RenderDocService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderDocService::_initializeService()
    {
        HMODULE renderDocModule = ::GetModuleHandleW( L"renderdoc.dll" );

        if( renderDocModule == NULL )
        {
            return false;
        }

        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress( renderDocModule, "RENDERDOC_GetAPI" );

        if( RENDERDOC_GetAPI == NULL  )
        {
            return false;
        }
         
        RENDERDOC_API_1_1_2 * rdoc_api = nullptr;
        if( RENDERDOC_GetAPI( eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api ) != 1 )
        {
            return false;
        }

        m_renderDocAPI = rdoc_api;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocService::_finalizeService()
    {
        m_renderDocAPI = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
