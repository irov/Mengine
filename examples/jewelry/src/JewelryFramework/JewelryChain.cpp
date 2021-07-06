#include "JewelryChain.h"

#include "Kernel/BezierHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryChain::JewelryChain()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryChain::~JewelryChain()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryChain::addJewelry( const JewelryPtr & _jewelry )
    {
        m_jewelries.emplace_back( _jewelry );
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryChain::clear()
    {
        m_jewelries.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryChain::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        if( m_jewelries.empty() == true )
        {
            return;
        }
    }
}