#	pragma once

#	define D3D_OVERLOADS

// Precompiler options
#	include "D3D7Prerequisites.h"

namespace Menge
{
    /** Encapsulates a Direct3D Device. Utility class for D3DRenderSystem. */
    class _MengeD3DExport D3DDevice
    {
    public:
        // Constructors
        D3DDevice(); // Default
        D3DDevice(const D3DDevice &ob); // Copy
        D3DDevice(LPDIRECT3D7 lpDirect3D, LPSTR lpDeviceDesc, LPSTR lpDeviceName,
                    LPD3DDEVICEDESC7 lpD3DDeviceDesc);
        ~D3DDevice();
        void cleanup(void);
        void logCaps(void) const;

        /** Creates a new per-rendering surface device */
        LPDIRECT3DDEVICE7 createDevice(LPDIRECTDRAWSURFACE7 renderTarget);
        LPDIRECT3D7 getID3D(void);

        // Overloaded operator =
        D3DDevice operator=(const D3DDevice &orig);

        // Information accessors
        const String& getDeviceName() const;
        const String& getDeviceDescription() const;
        bool isHardwareAccelerated() const;
        bool NeedsZBuffer() const;

        void createDepthBuffer(LPDIRECTDRAWSURFACE7 renderTarget);


        friend static HRESULT CALLBACK EnumZBuffersCallback(DDPIXELFORMAT* pddpf,
            VOID* pFormats);


        // Capability access
        bool canMipmap() const;
        bool canBilinearFilter() const;
        bool canTrilinearFilter() const;
        unsigned int renderBitDepth() const;
        unsigned int zBufferBitDepth() const;
        bool canHWTransformAndLight() const;
        unsigned int maxSinglePassTextureLayers() const;
        unsigned short stencilBufferBitDepth() const;

	private:
		String m_deviceName;
		String m_deviceDescription;
		D3DDEVICEDESC7 m_D3DDeviceDesc;
		bool m_isHardwareAccelerated;
		bool m_needsZBuffer;
		std::vector<DDPIXELFORMAT> m_depthBufferFormats;
		unsigned short m_stencilBufferDepth;



		LPDIRECT3D7 lpD3D;
		D3DVIEWPORT7 m_viewport;
		D3DRECT rcViewportRect;

		DDDriver* m_parentDriver;

    };
} // Namespace Menge
