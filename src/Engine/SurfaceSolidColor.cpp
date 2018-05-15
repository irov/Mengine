#include "SurfaceSolidColor.h"

#include "Interface/RenderSystemInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    SurfaceSolidColor::SurfaceSolidColor()
        : m_size(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
    SurfaceSolidColor::~SurfaceSolidColor()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSolidColor::setSolidColor( const ColourValue & _color )
	{
        m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & SurfaceSolidColor::getSolidColor() const
	{
		return m_color;
	}
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSolidColor::setSolidSize( const mt::vec2f & _size )
    {
        m_size = _size;

        this->invalidateMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSolidColor::getSolidSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSolidColor::correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in )
    {
        (void)_index;

        _out = _in;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceSolidColor::_update( float _current, float _timing )
	{	
        (void)_current;
        (void)_timing;

        return false;
	}
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSolidColor::getMaxSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSolidColor::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSolidColor::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceSolidColor::getUVCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceSolidColor::getUV( uint32_t _index ) const
    {
        (void)_index;

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceSolidColor::getColor() const
    {        
        return m_color;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceSolidColor::_updateMaterial() const
	{	
        bool solid = m_color.isSolid();

		RenderMaterialInterfacePtr material = this->makeSolidMaterial( solid );

		if( material == nullptr )
		{
			LOGGER_ERROR("SurfaceSolidColor::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
}
