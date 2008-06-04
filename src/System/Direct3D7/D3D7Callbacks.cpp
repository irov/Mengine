
#include "D3D7Callbacks.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    HRESULT WINAPI EnumModesCallback(
                                LPDDSURFACEDESC2 lpDDSurfaceDesc,
                                LPVOID lpContext)
    {

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    HRESULT CALLBACK D3DEnumDevicesCallback(
                                    GUID FAR* lpGuid,
                                    LPSTR lpDeviceDescription,
                                    LPSTR lpDeviceName,
                                    LPD3DDEVICEDESC7 lpD3DHWDeviceDesc,
                                    LPD3DDEVICEDESC7 lpD3DHELDeviceDesc,
                                    LPVOID lpContext)
    {

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
