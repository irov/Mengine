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

#include "Kernel/Factorable.h"
#include "Kernel/Unknowable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Rect.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/Color.h"
#include "Kernel/Pointer.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"

#include "Config/Typedef.h"

#include "math/mat4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderSystemInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "RenderSystem" )

    public:
        virtual ERenderPlatform getRenderPlatformType() const = 0;
        virtual const ConstString & getRenderPlatformName() const = 0;

    public:
        virtual bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _depth
            , bool _waitForVSync, int32_t _FSAAType, int32_t _FSAAQuality, uint32_t _MultiSampleCount ) = 0;

    public:
        virtual void setProjectionMatrix( const mt::mat4f & _projection ) = 0;
        virtual void setViewMatrix( const mt::mat4f & _view ) = 0;
        virtual void setWorldMatrix( const mt::mat4f & _view ) = 0;

    public:
        virtual void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) = 0;

    public:
        virtual RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc ) = 0;
        virtual bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) = 0;

    public:
        virtual RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc ) = 0;
        virtual bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) = 0;

    public:
        virtual RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc ) = 0;
        virtual RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) = 0;
        virtual RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) = 0;
        virtual RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc ) = 0;

        virtual void setProgram( const RenderProgramInterfacePtr & _program ) = 0;
        virtual void updateProgram( const RenderProgramInterfacePtr & _program ) = 0;
        virtual RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc ) = 0;
        virtual bool setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable ) = 0;

    public:
        virtual void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex, uint32_t _minIndex, uint32_t _vertexCount, uint32_t _startIndex, uint32_t _indexCount ) = 0;

    public:
        virtual void setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture ) = 0;
        virtual void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) = 0;
        virtual void setTextureFactor( uint32_t _color ) = 0;
        virtual void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate ) = 0;
        virtual void setCullMode( ECullMode _mode ) = 0;
        virtual void setDepthBufferTestEnable( bool _depthTest ) = 0;
        virtual void setDepthBufferWriteEnable( bool _depthWrite ) = 0;
        virtual void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) = 0;
        virtual void setFillMode( EFillMode _mode ) = 0;
        virtual void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) = 0;
        virtual void setAlphaBlendEnable( bool _alphaBlend ) = 0;

    public:
        virtual void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) = 0;

    public:
        virtual RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc ) = 0;

        virtual RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc ) = 0;
        virtual RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc ) = 0;

        virtual RenderImageInterfacePtr createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool beginScene() = 0;
        virtual void endScene() = 0;
        virtual void swapBuffers() = 0;
        virtual void clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil ) = 0;

        virtual void setScissor( const Viewport & _viewport ) = 0;
        virtual void removeScissor() = 0;

        virtual void setViewport( const Viewport & _viewport ) = 0;

        virtual void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) = 0;

        virtual void onWindowMovedOrResized() = 0;
        virtual void onWindowClose() = 0;

        virtual void onWindowChangeFullscreenPrepare( bool _fullscreen ) = 0;
        virtual bool onWindowChangeFullscreen( bool _fullscreen ) = 0;

        virtual void setVSync( bool _vSync ) = 0;

    public:
        virtual bool supportTextureFormat( EPixelFormat _format ) const = 0;
        virtual bool supportTextureNonPow2() const = 0;
        virtual uint32_t getMaxCombinedTextureImageUnits() const = 0;

    public:
        virtual uint32_t getAvailableTextureMemory() const = 0;
        virtual uint32_t getTextureMemoryUse() const = 0;
        virtual uint32_t getTextureCount() const = 0;

    public:
        virtual UnknownPointer getRenderSystemExtention() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDER_SYSTEM()\
    ((Mengine::RenderSystemInterface*)SERVICE_GET(Mengine::RenderSystemInterface))    
//////////////////////////////////////////////////////////////////////////
