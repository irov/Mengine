#	pragma once

//#include "OgrePrerequisites.h"
//#include "OgreDefaultHardwareBufferManager.h"
//#include "OgreException.h"

namespace Menge
{


	// Specialisation of HardwareBufferManager for D3D7.
	// @remarks
	// This class uses the software emulated buffers because D3D7's
	// hardware vertex buffers are far too primitive. The only reason we
	// override this is because D3D7 only allows 16-bit indexes so we have 
	// to prevent 32-bit index buffers being created.
	
	/*class D3D7HardwareBufferManager// : public DefaultHardwareBufferManager
	{
    public:
        D3D7HardwareBufferManager() {}
        ~D3D7HardwareBufferManager() {}
		/// Create a hardware index buffer
		HardwareIndexBufferSharedPtr
			createIndexBuffer( HardwareIndexBuffer::IndexType _itype, std::size_t _numIndexes, 
				HardwareBuffer::Usage _usage, bool _useShadowBuffer = false )
        {
            if (_itype == HardwareIndexBuffer::IT_32BIT)
            {
                OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Direct3D7 only supports 16-bit index buffers, "
                    "you should upgrade to a more recent Direct3D RenderSystem if you wish to use "
                    "32-bit indexes.", "D3D7HardwareBufferManager::createIndexBuffer");
            }
            return DefaultHardwareBufferManager::createIndexBuffer(_itype, _numIndexes, _usage, _useShadowBuffer);
        }

    };*/


}
