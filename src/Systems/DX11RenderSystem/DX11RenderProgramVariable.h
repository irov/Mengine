#pragma once

#include "Interface/RenderProgramVariableInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderProgramVariable
        : public RenderProgramVariableInterface
        , public Factorable
    {
    public:
        DX11RenderProgramVariable();
        ~DX11RenderProgramVariable() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount );
        void finalize();

    public:
        void setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;
        void updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count ) override;

    public:
        bool apply( ID3D11Device * _pD3DDevice, const RenderProgramInterfacePtr & _program );

    public:

		// D3D11 uses buffers registers (13 nums maximum)
		// each variable representing a buffer

 /*       enum EProgramVariableType
        {
            EPVT_FLOAT,
            EPVT_INTEGER,
            EPVT_BOOLEAN
        };
*/
		// we still can update whole buffer with variable index if we need not only float4 variable
        struct ProgramVariableDesc
        {
            //EProgramVariableType type;
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

		typedef Vector<ID3D11Buffer*> VectorBuffers;
		VectorBuffers m_vertexBuffers;
		VectorBuffers m_pixelBuffers;

		bool m_vertexBufferUpdate;
		bool m_pixelBufferUpdate;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderProgramVariable, RenderProgramVariableInterface> DX11RenderProgramVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}