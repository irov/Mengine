#include "Resolution.h"

#include "Config/StdMath.h"

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
        float widthF = this->getWidthF();
        float heightF = this->getHeightF();

        _size->x = widthF;
        _size->y = heightF;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::calcInvSize( mt::vec2f * const _size ) const
    {
        float widthF = this->getWidthF();
        float heightF = this->getHeightF();

        _size->x = 1.f / widthF;
        _size->y = 1.f / heightF;
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
    void Resolution::scaleTo( const mt::vec2f & _scale, Resolution * const _resolution ) const
    {
        float widthF = this->getWidthF();
        float heightF = this->getHeightF();

        float scale_widthF = widthF * _scale.x;
        float scale_heightF = heightF * _scale.y;

        _resolution->m_width = (uint32_t)scale_widthF;
        _resolution->m_height = (uint32_t)scale_heightF;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resolution::adaptiveHeight( uint32_t _availableHeight, Resolution * const _adaptiveResolution ) const
    {
        float aspect = this->getAspectRatio();

        float best_width = 0.f;
        float best_height = 0.f;
        float min_fraction = 1.f;

        for( float i = 0.f; i < 128.f; i += 2.f )
        {
            float probe_height = _availableHeight - i;
            float probe_width = probe_height * aspect;

            uint32_t probe_width_int = (uint32_t)probe_width;

            if( probe_width_int % 2 == 0 )
            {
                float integral;
                float fraction = StdMath::modff( probe_width, &integral );

                float inv_fraction = 1.f - fraction;

                if( fraction < min_fraction )
                {
                    min_fraction = fraction;
                    best_width = probe_width;
                    best_height = probe_height;
                }

                if( inv_fraction < min_fraction )
                {
                    min_fraction = inv_fraction;
                    best_width = probe_width + 1.f;
                    best_height = probe_height;
                }
            }
        }

        *_adaptiveResolution = Resolution( (uint32_t)best_width, (uint32_t)best_height );
    }
    //////////////////////////////////////////////////////////////////////////
}
