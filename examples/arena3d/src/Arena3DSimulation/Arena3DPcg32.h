#pragma once

#include "kinefix/kinefix.hpp"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class Pcg32
    {
    public:
        Pcg32();
        Pcg32( uint64_t _seed, uint64_t _stream );

        void seed( uint64_t _seed, uint64_t _stream );
        uint32_t next();
        uint32_t bounded( uint32_t _bound );

        uint64_t state() const;
        uint64_t stream() const;
        void restore( uint64_t _state, uint64_t _stream );

    private:
        kf_pcg32_t m_random{0, 1};
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
