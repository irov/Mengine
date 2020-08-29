#pragma once

#include "Config/UniqueId.h"
#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAffectorType
    {
        ETA_POSITION,
        ETA_ANGLE,
        ETA_SCALE,
        ETA_COLOR,
        ETA_VISIBILITY,
        ETA_SCRIPT,
        ETA_USER,
        ETA_END,
        __ETA_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    const Char * affectorToString( uint32_t _event );
    //////////////////////////////////////////////////////////////////////////
}