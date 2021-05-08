#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderVertexAttribute );

    public:
        OpenGLRenderVertexAttribute();
        ~OpenGLRenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        bool compile( GLuint _program );

    public:
        void enable();
        void disable();

    protected:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    protected:
        void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ConstString m_name;
        uint32_t m_elementSize;

        struct AttributeDesc
        {
            ConstString uniform;
            GLint location;

            uint32_t size;
            EVertexAttributeType type;
            uint32_t stride;
            uint32_t offset;

            GLboolean normalized;
        };

        typedef Vector<AttributeDesc> VectorAttribute;
        VectorAttribute m_attributes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderVertexAttribute, RenderVertexAttributeInterface> OpenGLRenderVertexAttributePtr;
    //////////////////////////////////////////////////////////////////////////
}
