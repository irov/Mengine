#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
    public:
        MockupRenderFragmentShader();
        ~MockupRenderFragmentShader() override;

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
    typedef IntrusivePtr<MockupRenderFragmentShader, RenderFragmentShaderInterface> MockupRenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}