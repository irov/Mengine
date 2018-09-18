#pragma once

#include "Interface/RenderInterface.h"

#include "Kernel/Visitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVisitor
        : public Visitor
    {
    public:
        RenderVisitor();
        ~RenderVisitor() override;

    public:
        void setRenderContext( const RenderContext * _context );
        const RenderContext * getRenderContext() const;

    protected:
        const RenderContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVisitor> RenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}