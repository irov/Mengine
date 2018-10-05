#pragma once

#include "DebugRenderInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DebugRenderService
        : public ServiceBase<DebugRenderServiceInterface>
    {
    public:
        DebugRenderService();
        ~DebugRenderService() override;

    public:
        void render( const NodePtr & _node, const RenderContext * _context, bool _external ) override;

    public:
        void addRenderVisitor( const RenderVisitorPtr & _renderVisitor ) override;
        void removeRenderVisitor( const RenderVisitorPtr & _renderVisitor ) override;

    protected:
        typedef Vector<RenderVisitorPtr> VectorRenderVisitor;
        VectorRenderVisitor m_renderVisitors;
    };
}