#pragma once

#include "MosaicInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Vector.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"

#include "Interface/RenderMaterialInterface.h"

#include "MosaicRenderScissor.h"

#include "Mosaic/Mosaic.hpp"

namespace Mengine
{
    class MosaicModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( MosaicModule );

    public:
        MosaicModule();
        ~MosaicModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update() override;
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void _handleFocus( bool _focus ) override;
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;
        bool _handleTextEvent( const InputTextEvent & _event ) override;
        bool _handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool _handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool _handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        Mosaic::PointerState & acquirePointer_( uint32_t _touchId );
        void updateModifiers_( const InputSpecialData & _special );
        void clearInputEdges_();
        const MosaicRenderScissorPtr & acquireScissor_( const Mosaic::Rect & _clip, const RenderScissorInterface * _parent ) const;

    protected:
        Mosaic::Context * m_context;
        Mosaic::Input m_input;
        Mosaic::InputCapture m_inputCapture;

        VectorMosaicProviders m_frameProviders;

        const Mosaic::RenderMesh * m_mesh;

        double m_timestamp;

        mt::vec2f m_contentResolution;

        typedef Vector<RenderVertex2D> VectorMosaicVertices;
        typedef Vector<RenderIndex> VectorMosaicIndices;
        struct MosaicScissorDesc
        {
            Mosaic::Rect clip;
            MosaicRenderScissorPtr scissor;
        };

        typedef Vector<MosaicScissorDesc> VectorMosaicScissors;
        typedef Vector<RenderMaterialInterfacePtr> VectorMosaicMaterials;

        typedef Vector<VectorMosaicVertices> VectorMosaicVertexBuffers;
        typedef Vector<VectorMosaicIndices> VectorMosaicIndexBuffers;

        mutable VectorMosaicVertexBuffers m_vertices;
        mutable VectorMosaicIndexBuffers m_indices;
        mutable VectorMosaicScissors m_scissors;
        mutable VectorMosaicMaterials m_materials;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MosaicModule, ModuleInterface> MosaicModulePtr;
    //////////////////////////////////////////////////////////////////////////
}
