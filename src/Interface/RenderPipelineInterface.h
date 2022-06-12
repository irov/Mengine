#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderProgramVariableInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderExternalInterface.h"

#include "Kernel/RenderContext.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderPipelineInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void addRenderMesh( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _vertexCount, uint32_t _indexCount
            , uint32_t _baseVertexIndex, uint32_t _startIndex, const DocumentPtr & _doc ) = 0;

        virtual void addRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) = 0;

        virtual void addRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) = 0;

        virtual void addRenderLine( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) = 0;

    public:
        virtual void addRenderExternal( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderExternalInterfacePtr & _external
            , const DocumentPtr & _doc ) = 0;

    public:
        virtual void addDebugRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount, const DocumentPtr & _doc ) = 0;

        virtual void addDebugRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount, const DocumentPtr & _doc ) = 0;

    public:
        virtual void endDebugLimitRenderObjects() = 0;
        virtual void increfDebugLimitRenderObjects() = 0;
        virtual bool decrefDebugLimitRenderObjects() = 0;

    public:
        virtual void enableDebugFillrateCalcMode( bool _enable ) = 0;
        virtual bool isDebugFillrateCalcMode() const = 0;

        virtual void enableDebugStepRenderMode( bool _enable ) = 0;
        virtual bool isDebugStepRenderMode() const = 0;

    public:
        virtual void prepare() = 0;
        virtual void flush() = 0;
        virtual void batch() = 0;
        virtual void render() = 0;
        virtual void clear() = 0;
        virtual bool isEmpty() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderPipelineInterface> RenderPipelineInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}