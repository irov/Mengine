#include "DecoderRenderImageProvider.h"

#include "Interface/RenderTextureServiceInterface.h"

#include "DecoderRenderImageLoader.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProvider::DecoderRenderImageProvider()
        : m_codecFlags( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProvider::~DecoderRenderImageProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    void DecoderRenderImageProvider::initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags )
    {
        m_content = _content;
        m_codecFlags = _codecFlags;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr DecoderRenderImageProvider::getLoader( const DocumentInterfacePtr & _doc ) const
    {
        RenderImageLoaderInterfacePtr loader = RENDERTEXTURE_SERVICE()
            ->createDecoderRenderImageLoader( m_content, m_codecFlags, _doc );

        return loader;
    }
    //////////////////////////////////////////////////////////////////////////
}
