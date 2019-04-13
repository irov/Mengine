#include "SurfaceSolidColor.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceSolidColor::SurfaceSolidColor()
        : m_size( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceSolidColor::~SurfaceSolidColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceSolidColor::setSolidColor( const Color & _color )
    {
        m_color = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceSolidColor::getSolidColor() const
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
    void SurfaceSolidColor::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out )
    {
        MENGINE_UNUSED( _index );

        *_out = _in;
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
        MENGINE_UNUSED( _index );

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceSolidColor::getColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceSolidColor::_updateMaterial() const
    {
        //ToDo MaterialContex
        //bool solid = m_color.isSolid();

        const RenderMaterialInterfacePtr & material = this->makeSolidMaterial( false, MENGINE_DOCUMENT_FUNCTION );

        if( material == nullptr )
        {
            LOGGER_ERROR( "'%s' m_material is NULL"
                , this->getName().c_str()
            );

            return nullptr;
        }

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
}
