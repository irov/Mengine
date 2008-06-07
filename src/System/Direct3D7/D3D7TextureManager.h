#	pragma once

#	include "Interface/TextureInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "D3D7Prerequisites.h"

//#include "OgreTextureManager.h"
//#include "D3D7Texture.h"

namespace Menge
{
    // Direct3D-specific implementation of a TextureManager
    class D3DTextureManager //: public TextureManagerInterface
    {
    public:
        D3DTextureManager( LPDIRECT3DDEVICE7 lpD3DDevice );
        virtual ~D3DTextureManager();

		// Restore the contents of all surfaces following a lost device.
		void reloadAfterLostDevice();

		PixelFormat getNativeFormat( ETextureType ttype, PixelFormat format, int usage );

		bool isHardwareFilteringSupported( ETextureType ttype, PixelFormat format, int usage,
			bool preciseFormatOnly = false );


    protected:
        /// @copydoc ResourceManager::createImpl
       /* Resource* createImpl(const String& name, ResourceHandle handle, 
            const String& group, bool isManual, ManualResourceLoader* loader, 
            const NameValuePairList* createParams);*/

        LPDIRECT3DDEVICE7 mlpD3DDevice;
    };
}
