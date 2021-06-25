#include "Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resolution::Resolution()
        : m_width( 0 )
        , m_height( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution::Resolution( uint32_t _width, uint32_t _height )
        : m_width( _width )
        , m_height( _height )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::setWidth( uint32_t _width )
    {
        m_width = _width;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::setHeight( uint32_t _height )
    {
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resolution::operator == ( const Resolution & _resolution ) const
    {
        return m_width == _resolution.m_width && m_height == _resolution.m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resolution::operator != ( const Resolution & _resolution ) const
    {
        return m_width != _resolution.m_width || m_height != _resolution.m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float Resolution::getAspectRatio() const
    {
        float width = this->getWidthF();
        float height = this->getHeightF();

        float aspect = width / height;

        return aspect;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::calcSize( mt::vec2f * const _size ) const
    {
        float width = this->getWidthF();
        float height = this->getHeightF();

        _size->x = width;
        _size->y = height;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::calcInvSize( mt::vec2f * const _size ) const
    {
        float width = this->getWidthF();
        float height = this->getHeightF();

        _size->x = 1.f / width;
        _size->y = 1.f / height;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::calcScale( const Resolution & _resolution, mt::vec2f * const _scale ) const
    {
        mt::vec2f self_size;
        this->calcSize( &self_size );

        mt::vec2f other_size;
        _resolution.calcSize( &other_size );

        *_scale = self_size / other_size;
    }
    //////////////////////////////////////////////////////////////////////////
}
