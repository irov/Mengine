#pragma once

#include "Kernel/FilePath.h"

#include "Config/Lambda.h"

namespace Mengine
{
    typedef Lambda<bool( const FilePath & )> LambdaFilePath;
}