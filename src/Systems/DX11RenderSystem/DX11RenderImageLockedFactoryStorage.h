#pragma once

#include "DX11RenderImageLocked.h"

#include "Kernel/FactoryStorage.h"

namespace Mengine
{
    typedef FactoryStorage<DX11RenderImageLocked> DX11RenderImageLockedFactoryStorage;
}