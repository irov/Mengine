#pragma once

#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderVertexShader
        : public RenderVertexShaderInterface
        , public Factorable
    {
    public:
        MockupRenderVertexShader();
        ~MockupRenderVertexShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    protected:
        ConstString m_name;

        MemoryInterfacePtr m_memory;

        bool m_compile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderVertexShader> MockupRenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}