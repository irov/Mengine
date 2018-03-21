#   pragma once

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/MemoryInterface.h"

#   include "Core/ServantBase.h"

#include <d3d9.h>

namespace Mengine
{
	class DX9RenderVertexAttribute
		: public ServantBase<RenderVertexAttributeInterface>
	{
	public:
        DX9RenderVertexAttribute();
		~DX9RenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name );
        
    public:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    public:
        bool enable() override;
        void disable() override;

    public:
        void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

	protected:
        ConstString m_name;

        struct Attribute
        {
            ConstString uniform;

            uint32_t size;
            EVertexAttributeType type;
            uint32_t stride;
            uint32_t offset;

            bool normalized;
        };

        typedef stdex::vector<Attribute> TVectorAttribute;
        TVectorAttribute m_attributes;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DX9RenderVertexAttribute> DX9RenderVertexAttributePtr;
}