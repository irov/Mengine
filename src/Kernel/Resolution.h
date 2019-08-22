#pragma once

#include "Config/Typedef.h"

#include "math/vec2.h"

namespace Mengine
{
    class Resolution
    {
    public:
        Resolution();
        Resolution( uint32_t _width, uint32_t _height );

    public:
        void setWidth( uint32_t _width );
        MENGINE_INLINE uint32_t getWidth() const;

        void setHeight( uint32_t _height );
        MENGINE_INLINE uint32_t getHeight() const;

        MENGINE_INLINE float getWidthF() const;
        MENGINE_INLINE float getHeightF() const;

        float getAspectRatio() const;

        void calcSize( mt::vec2f & _size ) const;
        void calcInvSize( mt::vec2f & _size ) const;
        void calcScale( const Resolution & _resolution, mt::vec2f & _scale ) const;

    public:
        bool operator == ( const Resolution & _resolution ) const;

    protected:
        uint32_t m_width;
        uint32_t m_height;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Resolution::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Resolution::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Resolution::getWidthF() const
    {
        uint32_t width = this->getWidth();

        return (float)width;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Resolution::getHeightF() const
    {
        uint32_t height = this->getHeight();

        return (float)height;
    }
}
