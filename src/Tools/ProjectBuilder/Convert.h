#pragma once

#include "Config/Char.h"

#include "Kernel/Params.h"

namespace Mengine
{
    bool convert( const WChar * _fromPath, const WChar * _toPath, const WChar * _convertType, const Params & _params );
}