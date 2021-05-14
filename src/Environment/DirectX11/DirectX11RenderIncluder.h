#pragma once

#include "Config/Config.h"

#include <d3d11.h>

#include <wrl/client.h>

namespace Mengine
{
    template<class T>
    using DX11ResourcePtr = Microsoft::WRL::ComPtr<T>;
}