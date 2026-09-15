#include "MosaicRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicRender::MosaicRender()
        : m_providerId( MOSAIC_INVALID_PROVIDER_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicRender::~MosaicRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRender::setProvider( const LambdaMosaicProvider & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const MosaicRender::LambdaMosaicProvider & MosaicRender::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicRender::_activate()
    {
        if( m_provider == nullptr )
        {
            return true;
        }

        m_providerId = MOSAIC_SERVICE()
            ->addProvider( m_provider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRender::_deactivate()
    {
        if( m_providerId == MOSAIC_INVALID_PROVIDER_ID )
        {
            return;
        }

        MOSAIC_SERVICE()
            ->removeProvider( m_providerId );

        m_providerId = MOSAIC_INVALID_PROVIDER_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
