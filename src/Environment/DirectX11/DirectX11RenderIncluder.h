#pragma once

#include "Config/Config.h"

#include <d3d11.h>

#include <wrl/client.h>

namespace Mengine
{
    template<class T>
    using DX11ResourcePtr = Microsoft::WRL::ComPtr<T>;

    typedef DX11ResourcePtr<ID3D11DeviceContext> ID3D11DeviceContextPtr;
}