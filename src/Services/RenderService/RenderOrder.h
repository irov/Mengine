#pragma once

#include "Interface/RenderOrderInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderOrder
        : public RenderOrderInterface
    {
        DECLARE_FACTORABLE( RenderOrder );

    public:
        RenderOrder();
        ~RenderOrder() override;

    public:
        void setOrder( uint32_t _order );
        uint32_t getOrder() const;

    public:
        void beginOrder();
        void flushOrder( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context );

    public:
        void addRender( int32_t _index, RenderInterface * _render ) override;

    protected:
        uint32_t m_order;

        struct OrderDesc
        {
            int32_t index;
            RenderInterface * render;            
        };

        typedef Vector<OrderDesc> VectorRenders;
        VectorRenders m_renders;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderOrder, RenderOrderInterface> RenderOrderPtr;
    //////////////////////////////////////////////////////////////////////////
}