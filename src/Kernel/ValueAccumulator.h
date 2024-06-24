#pragma once

#include "Interface/EasingInterface.h"

namespace Mengine
{
    template<typename T>
    class ValueAccumulator
    {
    public:
        ValueAccumulator()
            : m_started( false )
            , m_time( 0.f )
        {
        }

        ~ValueAccumulator()
        {
        }

    public:
        void stop()
        {
            m_started = false;
        }

        bool isStarted() const
        {
            return m_started;
        }

        virtual bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * const _out, float * const _used ) = 0;

    protected:
        bool m_started;
        float m_time;

        T m_pos;
        T m_delta;
    };
}
