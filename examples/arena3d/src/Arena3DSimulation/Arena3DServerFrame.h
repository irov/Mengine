#pragma once

#include "Arena3DSimulation.h"

#include "Kernel/Data.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    struct ServerFrame
    {
        uint64_t tick = 0;
        uint64_t acknowledgedSequence = 0;
        StateChecksum checksum;
        VectorCommandEnvelope commands;
        VectorServerEvent events;
        Mengine::Data snapshot;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
