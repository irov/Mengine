#pragma once

#include "BaseDebugRender.h"

#include "Engine/TextField.h"

namespace Mengine
{
    class TextFieldDebugRender
        : public BaseDebugRenderT<TextField>
    {
    public:
        TextFieldDebugRender();
        ~TextFieldDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const TextField * _node, bool _hide ) override;
    };
}