#pragma once

#include "Interface/RenderProgramVariableInterface.h"

#include "Environment/DirectX11/DX11RenderIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderProgramVariable
        : public RenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderProgramVariable );

    public:
        DX11RenderProgramVariable();
        ~DX11RenderProgramVariable() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount );
        void finalize();

    public:
        void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        bool apply( const ID3D11DevicePtr & _pD3DDevice, const ID3D11DeviceContextPtr & _pImmediateContext, const RenderProgramInterfacePtr & _program );

    public:
        struct ProgramVariableDesc
        {
            uint32_t offset;
            uint32_t size;
            uint32_t count;
        };

    protected:
        typedef Vector<float> VectorDataFloats;
        VectorDataFloats m_vertexFloats;
        VectorDataFloats m_pixelFloats;

        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;

        typedef Vector<ID3D11Buffer *> VectorBuffers;
        VectorBuffers m_vertexBuffers;
        VectorBuffers m_pixelBuffers;

        bool m_vertexBufferUpdate;
        bool m_pixelBufferUpdate;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderProgramVariable, RenderProgramVariableInterface> DX11RenderProgramVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}