#	include "Kernel/ResourceImplement.h"
#	include "ResourceImageSolid.h"

#	include "Logger/Logger.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSolid );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSolid::ResourceImageSolid()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & ResourceImageSolid::getColor() const 
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageSolid * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSolid *>(_meta);
        
        m_size = metadata->get_Size_Value();
        m_color = metadata->get_Color_Value();

        m_maxSize = m_size;
		m_offset = mt::vec2f(0.f, 0.f);
        m_isAlpha = false;
        m_wrapU = true;
        m_wrapV = true;

        m_texture = nullptr;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSolid::_isValid() const 
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}