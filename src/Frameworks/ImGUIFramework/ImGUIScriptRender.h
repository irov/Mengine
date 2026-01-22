#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "ImGUIInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    class ImGUIScriptRender
        : public Factorable
        , public Renderable
        , protected BaseRender
        , public RenderDrawPrimitiveInterface
    {
        DECLARE_FACTORABLE( ImGUIScriptRender );

    public:
        ImGUIScriptRender();
        ~ImGUIScriptRender() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args, const ImGUIRenderProviderInterfacePtr & _imguiRenderProvider );

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;

        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;
    };
}
