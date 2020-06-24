#include "BaseRenderZOrder.h"

#include "Interface/RenderInterface.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseRenderZOrder::BaseRenderZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseRenderZOrder::~BaseRenderZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseRenderZOrder::initialize()
    {
        m_zRenderObjectsAfter.reserve( 1024 );
        m_zRenderObjectsBefore.reserve( 1024 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRenderZOrder::finalize()
    {
        m_zRenderObjectsAfter.clear();
        m_zRenderObjectsBefore.clear();

        m_zRenderObjectsAfter.shrink_to_fit();
        m_zRenderObjectsBefore.shrink_to_fit();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRenderZOrder::fetchZOrder( RenderInterface * _render, const RenderContext * _context )
    {
        _render->fetchZOrderWithChildren( RenderZOrderInterfacePtr::from( this ), _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRenderZOrder::addZOrderRender( int32_t _zIndex, int32_t _zOrder, const RenderInterface * _render, const RenderContext * _context )
    {
        ZRenderObject zro;
        zro.zIndex = _zIndex;
        zro.zOrder = _zOrder;
        zro.render = _render;
        zro.context = *_context;

        if( _zIndex > 0 )
        {
            m_zRenderObjectsAfter.emplace_back( zro );
        }
        else
        {
            m_zRenderObjectsBefore.emplace_back( zro );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRenderZOrder::flushZOrderRenderAfter( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        std::sort( m_zRenderObjectsAfter.begin(), m_zRenderObjectsAfter.end(), []( const ZRenderObject & _zro0, const ZRenderObject & _zro1 )
        {
            if( _zro0.zIndex < _zro1.zIndex )
            {
                return true;
            }
            else if( _zro0.zIndex > _zro1.zIndex )
            {
                return false;
            }

            if( _zro0.zOrder < _zro1.zOrder )
            {
                return true;
            }

            return false;
        } );

        for( const ZRenderObject & zro : m_zRenderObjectsAfter )
        {
            const RenderInterface * render = zro.render;

            render->render( _renderPipeline, &zro.context );
        }

        m_zRenderObjectsAfter.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRenderZOrder::flushZOrderRenderBefore( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        std::sort( m_zRenderObjectsBefore.begin(), m_zRenderObjectsBefore.end(), []( const ZRenderObject & _zro0, const ZRenderObject & _zro1 )
        {
            if( _zro0.zIndex < _zro1.zIndex )
            {
                return true;
            }
            else if( _zro0.zIndex > _zro1.zIndex )
            {
                return false;
            }

            if( _zro0.zOrder < _zro1.zOrder )
            {
                return true;
            }

            return false;
        } );

        for( const ZRenderObject & zro : m_zRenderObjectsBefore )
        {
            const RenderInterface * render = zro.render;

            render->render( _renderPipeline, &zro.context );
        }

        m_zRenderObjectsBefore.clear();
    }    
}