/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
#ifndef _D3DTexture_H__
#define _D3DTexture_H__

#include "OgreD3D7Prerequisites.h"
#include "OgreTexture.h"
#include "OgreRenderTarget.h"
#include "OgreRenderTexture.h"
#include "OgreRenderTargetListener.h"
#include "OgreD3D7HardwarePixelBuffer.h"

namespace Ogre {

    /** Direct3D7-specific texture resource implementation. 
    */
    class D3DTexture : public Texture
    {
    public:
        // Constructor, called from D3DTextureManager
        D3DTexture(ResourceManager* creator, const String& name, ResourceHandle handle,
            const String& group, bool isManual, ManualResourceLoader* loader, 
            IDirect3DDevice7 * lpDirect3dDevice);
        virtual ~D3DTexture();

        //virtual void loadImage( const Image &img );
		virtual void loadImage3D( const Image imgs[]);
        virtual void copyToTexture(TexturePtr& target );

        /// D3D-specific member that returns the underlying surface.
        LPDIRECTDRAWSURFACE7 getDDSurface(void);

		/// @copydoc Texture::getBuffer
		HardwarePixelBufferSharedPtr getBuffer(size_t face, size_t mipmap);

		/// Static pixelformat functions
		static D3DX_SURFACEFORMAT OgreFormat_to_D3DXFormat( PixelFormat format );
		static PixelFormat closestD3DXFormat( PixelFormat format );
		static bool OgreFormat_to_DDPixelFormat( PixelFormat format, DDPIXELFORMAT & out );

		/// Restore this texture from a lost device
		void restoreFromLostDevice(void);

    protected:
        IDirect3DDevice7 * mD3DDevice;       ///< A pointer to the Direct3D device.
        IDirectDrawSurface7 * mSurface;      ///< Surface of the (first) device-specific texture.
		/// cube texture individual face names
		String mCubeFaceNames[6];
		/// Vector of pointers to subsurfaces
		typedef std::vector<HardwarePixelBufferSharedPtr> SurfaceList;
		SurfaceList	mSurfaceList;
		/// Are we restoring from a lost device?
		bool mRestoring;

	

        /// @copydoc Resource::loadImpl
        void loadImpl(void);
		/// @copydoc Texture::createInternalResourcesImpl
		void createInternalResourcesImpl(void);
        /// @copydoc Resource::freeInternalResourcesImpl
        void freeInternalResourcesImpl(void);
		
		void createSurface2D(void);
		void createSurface3D(void);

		/// internal method, return a D3D pixel format for texture creation
		void _chooseD3DFormat(DDPIXELFORMAT &ddpf);

		/// internal method, construct full cube texture face names from a given string
		void _constructCubeFaceNames(const String name);

		/// internal method, the cube map face name for the spec. face index
		String _getCubeFaceName(unsigned char face) const
		{ assert(face < 6); return mCubeFaceNames[face]; }

		// Create the list of surfaces
		void _createSurfaceList();
    };

    /** Specialisation of SharedPtr to allow SharedPtr to be assigned to D3DTexturePtr 
    @note Has to be a subclass since we need operator=.
    We could templatise this instead of repeating per Resource subclass, 
    except to do so requires a form VC6 does not support i.e.
    ResourceSubclassPtr<T> : public SharedPtr<T>
    */
    class D3DTexturePtr : public SharedPtr<D3DTexture> 
    {
    public:
        D3DTexturePtr() : SharedPtr<D3DTexture>() {}
        explicit D3DTexturePtr(D3DTexture* rep) : SharedPtr<D3DTexture>(rep) {}
        D3DTexturePtr(const D3DTexturePtr& r) : SharedPtr<D3DTexture>(r) {} 
        D3DTexturePtr(const ResourcePtr& r) : SharedPtr<D3DTexture>()
        {
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
            pRep = static_cast<D3DTexture*>(r.getPointer());
            pUseCount = r.useCountPointer();
            if (pUseCount)
            {
                ++(*pUseCount);
            }
        }

        /// Operator used to convert a ResourcePtr to a D3DTexturePtr
        D3DTexturePtr& operator=(const ResourcePtr& r)
        {
            if (pRep == static_cast<D3DTexture*>(r.getPointer()))
                return *this;
            release();
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
            pRep = static_cast<D3DTexture*>(r.getPointer());
            pUseCount = r.useCountPointer();
            if (pUseCount)
            {
                ++(*pUseCount);
            }
            return *this;
        }
        /// Operator used to convert a TexturePtr to a D3DTexturePtr
        D3DTexturePtr& operator=(const TexturePtr& r)
        {
            if (pRep == static_cast<D3DTexture*>(r.getPointer()))
                return *this;
            release();
            pRep = static_cast<D3DTexture*>(r.getPointer());
            pUseCount = r.useCountPointer();
            if (pUseCount)
            {
                ++(*pUseCount);
            }
            return *this;
        }
    };


    /// D3D7 implementation of RenderTexture
    class D3D7RenderTexture : public RenderTexture
    {
    public:
        D3D7RenderTexture( const String &name, D3D7HardwarePixelBuffer *buffer )
			: RenderTexture( buffer, 0 )
        {
            /*mPrivateTex = TextureManager::getSingleton().createManual(mName + 
                "_PRIVATE##", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                texType, mWidth, mHeight, 0, internalFormat, TU_RENDERTARGET);
            mPrivateTex->createInternalResources();*/
			mName = name;
		}

		void rebind(D3D7HardwarePixelBuffer *buffer)
		{
			mBuffer = buffer;
			mWidth = mBuffer->getWidth();
			mHeight = mBuffer->getHeight();
			mColourDepth = Ogre::PixelUtil::getNumElemBits(mBuffer->getFormat());
		}

		virtual ~D3D7RenderTexture()
		{
			/*mPrivateTex->unload();
			TextureManager::getSingleton().remove(mPrivateTex->getName());*/
		}

		bool requiresTextureFlipping() const { return false; }

        virtual void getCustomAttribute( const String& name, void* pData )
        {
            if( name == "DDBACKBUFFER" )
            {
                LPDIRECTDRAWSURFACE7 *pSurf = (LPDIRECTDRAWSURFACE7*)pData;

                *pSurf = static_cast<D3D7HardwarePixelBuffer*>(mBuffer)->getSurface();
                return;
            }
            else if( name == "DDFRONTBUFFER" )
            {
                LPDIRECTDRAWSURFACE7 *pSurf = (LPDIRECTDRAWSURFACE7*)pData;

                //*pSurf = mPrivateTex->getDDSurface();
				*pSurf = static_cast<D3D7HardwarePixelBuffer*>(mBuffer)->getSurface();
               return;
            }
            else if( name == "HWND" )
            {
                HWND *pHwnd = (HWND*)pData;

                *pHwnd = NULL;
                return;
            }
            else if( name == "isTexture" )
            {
                bool *b = reinterpret_cast< bool * >( pData );
                *b = true;

                return;
            }
			else if(name == "BUFFER")
			{
				*static_cast<HardwarePixelBuffer**>(pData) = mBuffer;
				return;
			}
        }
     //   virtual void writeContentsToFile( const String & filename ) {}

    //protected:
        /// The texture to which rendering takes place.
       // D3DTexturePtr mPrivateTex;

    /*protected:
        virtual void _copyToTexture()
        {
            // Copy the newly-rendered data to the public texture surface.
            mPrivateTex->copyToTexture( mTexture );
        }*/
    };
}

#endif
