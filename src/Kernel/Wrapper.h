#pragma once

#include "Config/Config.h"

namespace Mengine
{
    template<class T>
    class Wrapper
    {
    public:
        Wrapper( bool _value )
            : m_value( _value )
        {
        }

    public:
        operator T() const
        {
            return m_value;
        }

        operator T & ()
        {
            return m_value;
        }

    protected:
        bool m_value;
    };
}