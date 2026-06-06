#include "PostProcessFx3DRenderTarget.h"

#include "PostProcessFx3D.h"

#include "Interface/RenderPipelineInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PostProcessFx3DRenderTarget::PostProcessFx3DRenderTarget( PostProcessFx3D * _owner )
        : m_owner( _owner )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PostProcessFx3DRenderTarget::~PostProcessFx3DRenderTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3DRenderTarget::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderMaterialInterfacePtr & material = m_owner->getMaterial();
        MENGINE_ASSERTION_MEMORY_PANIC( material, "post-process fx render without material" );

        const RenderVertex2D * vertices = m_owner->getVerticesWM();
        const RenderIndex * indices = m_owner->getIndices();
        const RenderProgramVariableInterfacePtr & programVariable = m_owner->getProgramVariable();
        MENGINE_ASSERTION_MEMORY_PANIC( programVariable, "post-process fx render without program variable" );

        _renderPipeline->addRenderObject( _context, material, programVariable
            , vertices, 4u
            , indices, 6u
            , nullptr, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}
