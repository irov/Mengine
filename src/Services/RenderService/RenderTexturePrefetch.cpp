#include "RenderTexturePrefetch.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetch::RenderTexturePrefetch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetch::~RenderTexturePrefetch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderTexturePrefetchState RenderTexturePrefetch::getState() const
    {
        if( m_job == nullptr )
        {
            return ERenderTexturePrefetchState::ERTPS_CANCELLED;
        }

        ERenderTexturePrefetchState state = m_job->getState();

        return state;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & RenderTexturePrefetch::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTexturePrefetch::getTexture() const
    {
        if( m_job == nullptr )
        {
            return nullptr;
        }

        RenderTextureInterfacePtr texture = m_job->getTexture();

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetch::initialize( const RenderTexturePrefetchJobPtr & _job, const ContentInterfacePtr & _content, const LambdaRenderTexturePrefetch & _callback )
    {
        m_job = _job;
        m_content = _content;
        m_callback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetch::cancel()
    {
        m_job = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetch::notify()
    {
        ERenderTexturePrefetchState state = this->getState();

        if( state != ERenderTexturePrefetchState::ERTPS_RESIDENT && state != ERenderTexturePrefetchState::ERTPS_FAILED && state != ERenderTexturePrefetchState::ERTPS_CANCELLED )
        {
            return;
        }

        LambdaRenderTexturePrefetch notification = StdUtility::move( m_callback );
        m_callback = nullptr;

        if( notification != nullptr )
        {
            RenderTexturePrefetchInterfacePtr request = RenderTexturePrefetchInterfacePtr::from( this );
            notification( request );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetch::finalize()
    {
        m_callback = nullptr;
        this->cancel();
    }
    //////////////////////////////////////////////////////////////////////////
}
