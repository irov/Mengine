#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"

#include "Core/ServantBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderVertexAttribute
        : public ServantBase<RenderVertexAttributeInterface>
    {
    public:
        OpenGLRenderVertexAttribute();
        ~OpenGLRenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name );

    public:
        void bind( GLuint _program );

    protected:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    protected:
        void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;
        
    protected:
        bool enable() override;
        void disable() override;

    protected:
        ConstString m_name;

        struct Attribute
        {
            ConstString uniform;

            uint32_t size;
            EVertexAttributeType type;
            uint32_t stride;
            uint32_t offset;

            GLboolean normalized;
        };

        typedef stdex::vector<Attribute> TVectorAttribute;
        TVectorAttribute m_attributes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderVertexAttribute> OpenGLRenderVertexAttributePtr;
}
