#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EAffectorType
    {
        EAFFECTORTYPE_NONE,
        EAFFECTORTYPE_POSITION,
        EAFFECTORTYPE_ORIGIN,
        EAFFECTORTYPE_ANGLE,
        EAFFECTORTYPE_SCALE,
        EAFFECTORTYPE_COLOR,
        EAFFECTORTYPE_VISIBILITY,
        EAFFECTORTYPE_VOLUME,
        EAFFECTORTYPE_USER
    };
}