#pragma once

#include "metabuf/Metaconvert.hpp"

namespace Metacode
{
    class MetaprotocolGenerator
        : public Metabuf::ProtocolGenerator
    {
    public:
        void generate( Metabuf::ProtocolInterface * _protocol ) const override;
    };
}
