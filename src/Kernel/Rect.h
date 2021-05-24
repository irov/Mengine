#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Rect
    {
    public:
        Rect();
        Rect( uint32_t _left, uint32_t _top, uint32_t _right, uint32_t _bottom );

    public:
        MENGINE_INLINE uint32_t getOffsetX() const;
        MENGINE_INLINE uint32_t getOffsetY() const;
        MENGINE_INLINE uint32_t getWidth() const;
        MENGINE_INLINE uint32_t getHeight() const;

        MENGINE_INLINE float getWidthF() const;
        MENGINE_INLINE float getHeightF() const;

    public:
        bool full( uint32_t _width, uint32_t _height ) const;

    public:
        uint32_t left;
        uint32_t top;
        uint32_t right;
        uint32_t bottom;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Rect::getOffsetX() const
    {
        return left;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Rect::getOffsetY() const
    {
        return top;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Rect::getWidth() const
    {
        return this->right - this->left;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Rect::getHeight() const
    {
        return this->bottom - this->top;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Rect::getWidthF() const
    {
        uint32_t width = this->getWidth();

        return (float)width;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Rect::getHeightF() const
    {
        uint32_t height = this->getHeight();

        return (float)height;
    }
    //////////////////////////////////////////////////////////////////////////
}