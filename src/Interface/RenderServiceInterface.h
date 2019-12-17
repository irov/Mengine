#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderProgramInterface.h"
#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/RenderProgramVariableInterface.h"
#include "Interface/RenderBatchInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Rect.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/Pointer.h"
#include "Kernel/RenderPrimitive.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/RenderContext.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

#include "Config/Typedef.h"

#include "math/mat4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderServiceDebugInfo
    {
        uint32_t frameCount;
        uint32_t dips;

        float fillrate;
        uint32_t object;
        uint32_t triangle;
        uint32_t batch;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderService" )

    public:
        virtual VectorRenderVertex2D & getDebugRenderVertex2D( uint32_t _count ) = 0;
        virtual VectorRenderIndex & getDebugRenderIndex( uint32_t _count ) = 0;

    public:
        virtual bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen,
            int32_t _FSAAType, int32_t _FSAAQuality ) = 0;

        virtual void destroyRenderWindow() = 0;

        virtual void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) = 0;

    public:
        virtual const RenderBatchInterfacePtr & requestRenderBatch( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool beginScene( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;
        virtual void endScene( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;

        virtual bool beginRenderPass( const RenderVertexBufferInterfacePtr & _vertexBuffer,
            const RenderIndexBufferInterfacePtr & _indexBuffer,
            const RenderViewportInterfacePtr & _viewport,
            const RenderCameraInterfacePtr & _camera,
            const RenderTransformationInterfacePtr & _transformation,
            const RenderScissorInterfacePtr & _scissor,
            const RenderTargetInterfacePtr & _target,
            const RenderProgramVariableInterfacePtr & _programVariable ) = 0;

        virtual void endRenderPass( const RenderTargetInterfacePtr & _target ) = 0;

        virtual void renderPrimitives( RenderPrimitive * _primitives, uint32_t _count ) = 0;

    public:
        virtual void swapBuffers() = 0;

    public:
        virtual void setVSync( bool _vSync ) = 0;
        virtual bool getVSync() const = 0;

    public:
        virtual void onWindowClose() = 0;
        virtual void onDeviceLostPrepare() = 0;
        virtual void onDeviceLostRestore() = 0;
        virtual bool isWindowCreated() const = 0;

    public:
        virtual const RenderServiceDebugInfo & getDebugInfo() const = 0;
        virtual void resetFrameCount() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDER_SERVICE()\
    ((Mengine::RenderServiceInterface*)SERVICE_GET(Mengine::RenderServiceInterface))
//////////////////////////////////////////////////////////////////////////
