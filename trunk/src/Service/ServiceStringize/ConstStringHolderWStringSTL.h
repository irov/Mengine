#   pragma once

#   include "Core/ConstStringHolder.h"

namespace Menge
{
    class ConstStringHolderWStringSTL
        : public ConstStringHolderT<wchar_t>
    {
    public:
        ConstStringHolderWStringSTL();

    public:
        void setValue( const wchar_t * _value, size_t _size );

    protected:
        void _releaseString() override;

    protected:
        WString m_value;
    };
}