#pragma once

#include "Interface/RenderOrderInterface.h"

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
        void setIndex( int32_t _index ) override;
        int32_t getIndex() const override;

    protected:
        int32_t m_index;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderOrder, RenderOrderInterface> RenderOrderPtr;
    //////////////////////////////////////////////////////////////////////////
}