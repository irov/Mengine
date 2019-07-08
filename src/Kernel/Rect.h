#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    class Rect
    {
    public:
        Rect();
        Rect( uint32_t _left, uint32_t _top, uint32_t _right, uint32_t _bottom );

    public:
        MENGINE_INLINE uint32_t getWidth() const;
        MENGINE_INLINE uint32_t getHeight() const;

    public:
        bool full( uint32_t _width, uint32_t _height ) const;

    public:
        uint32_t left;
        uint32_t top;
        uint32_t right;
        uint32_t bottom;
    };
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
}