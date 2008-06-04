#	pragma once

// Precompiler options
#include "D3D7Prerequisites.h"

namespace Menge
{
    class _MengeD3DExport D3DDevice;
    
    // Encapsulates enumeration of Direct3D devices. Utility class for D3DRenderSystem.
    class  _MengeD3DExport D3DDeviceList
    {
    public:
		D3DDeviceList( LPDIRECT3D7 lpD3D ); // Only allowed Constructor
        ~D3DDeviceList();
        BOOL enumerate(); //Forces an enumeration / re-enumeration

        // List accessors
		D3DDevice* item( std::size_t index );
		std::size_t count();

        //Retrieves the 'best' 3D device type

        //This method returns the device type that is 'best' out of those available. The 3 types
        //that might be present are software emulation, hardware accelerated (HAL), and hardware transform
        //and light. Generally the latter are preferred over the former, however in some cases (mainly
        //very low-end cards) the hardware accelerated device type supports less features than the software
        //mode so may be preferred. In addition some HALs don't support the full range of rendering bit depths
        //(e.g. Matrox Millennium 1/2) so if a 32-bit colour depth is chosen, software mode must be used.
        D3DDevice* getBest(unsigned int minColourDepth);

        // Non-member callbacks
        friend static HRESULT CALLBACK D3DEnumDevicesCallback( LPSTR lpDeviceDescription,
                                    LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc,
                                    LPVOID lpContext);

		// Pointer to D3D interface
		LPDIRECT3D7 lpD3D;

	private:
		// STL list of drivers (Vector used to allow random access)
		std::vector<D3DDevice> m_deviceList;

		void _addDevice( LPSTR lpDeviceDesc, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc );
    };

}// Namespace Menge
