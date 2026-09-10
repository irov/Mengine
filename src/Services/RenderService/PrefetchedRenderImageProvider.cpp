#include "PrefetchedRenderImageProvider.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetchedRenderImageProvider::PrefetchedRenderImageProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetchedRenderImageProvider::~PrefetchedRenderImageProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchedRenderImageProvider::initialize( const ContentInterfacePtr & _content, const RenderImageLoaderInterfacePtr & _loader )
    {
        m_content = _content;
        m_loader = _loader;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & PrefetchedRenderImageProvider::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr PrefetchedRenderImageProvider::getLoader( const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return m_loader;
    }
    //////////////////////////////////////////////////////////////////////////
}
