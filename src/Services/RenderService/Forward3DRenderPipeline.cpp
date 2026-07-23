#include "Forward3DRenderPipeline.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/ProfilerHelper.h"

#include "Config/StdAlgorithm.h"

#include "math/mat4.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static float computePassViewZ( const RenderPass & _pass )
        {
            const RenderContext & ctx = _pass.context;

            if( ctx.camera == nullptr || ctx.transformation == nullptr )
            {
                return 0.f;
            }

            const mt::mat4f & wm = ctx.transformation->getTransformationWorldMatrix();
            const mt::mat4f & vm = ctx.camera->getCameraViewMatrix();

            mt::vec3f worldOrigin( wm.v3.x, wm.v3.y, wm.v3.z );

            mt::vec3f viewPos;
            mt::mul_v3_v3_m4( &viewPos, worldOrigin, vm );

            return viewPos.z;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Forward3DRenderPipeline::Forward3DRenderPipeline()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Forward3DRenderPipeline::~Forward3DRenderPipeline()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Forward3DRenderPipeline::render()
    {
        MENGINE_PROFILER_CATEGORY();

        StdAlgorithm::stable_sort( m_renderPasses.begin(), m_renderPasses.end()
            , []( const RenderPass & _l, const RenderPass & _r )
        {
            if( _l.zGroup != _r.zGroup )
            {
                return _l.zGroup < _r.zGroup;
            }

            bool lSingle = (_l.flags & RENDER_PASS_FLAG_SINGLE) == RENDER_PASS_FLAG_SINGLE;
            bool rSingle = (_r.flags & RENDER_PASS_FLAG_SINGLE) == RENDER_PASS_FLAG_SINGLE;

            if( lSingle != rSingle )
            {
                return rSingle;
            }

            if( lSingle == true )
            {
                float lz = Detail::computePassViewZ( _l );
                float rz = Detail::computePassViewZ( _r );

                return lz < rz;
            }

            return _l.zIndex < _r.zIndex;
        } );

        const RenderPrimitive * renderPrimitives = m_renderPrimitives.buff();

        for( const RenderPass & renderPass : m_renderPasses )
        {
            const RenderVertexBufferInterfacePtr & vertexBuffer = renderPass.vertexBuffer;
            const RenderIndexBufferInterfacePtr & indexBuffer = renderPass.indexBuffer;
            const RenderProgramVariableInterfacePtr & programVariable = renderPass.programVariable;

            const RenderContext * context = &renderPass.context;

            if( m_renderService->beginRenderPass( vertexBuffer, indexBuffer, programVariable, context ) == false )
            {
                continue;
            }

            const RenderDrawPrimitiveInterfacePtr & drawPrimitive = renderPass.drawPrimitive;

            if( drawPrimitive == nullptr )
            {
                m_renderService->drawPrimitives( renderPrimitives + renderPass.beginRenderObject, renderPass.countRenderObject );
            }
            else
            {
                drawPrimitive->onRenderDrawPrimitives( renderPrimitives + renderPass.beginRenderObject, renderPass.countRenderObject );
            }

            m_renderService->endRenderPass( context );
        }

        m_renderPrimitives.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
