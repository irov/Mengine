#	pragma once

#	include "Interface/TextureInterface.h"
#	include "Menge/PixelFormat.h"
#	include "D3D7Prerequisites.h"
//#include "OgreRenderTarget.h"
//#include "OgreRenderTexture.h"
//#include "OgreRenderTargetListener.h"
//#include "D3D7HardwarePixelBuffer.h"

namespace Menge
{
    // Direct3D7-specific texture resource implementation. 
    class D3DTexture : public TextureInterface
    {
    public:
        // Constructor, called from D3DTextureManager
        D3DTexture( const String& _name, IDirect3DDevice7 * lpDirect3dDevice );
        virtual ~D3DTexture();

        //virtual void loadImage( const Image &img );
		//virtual void loadImage3D( const Image imgs[]);
        void copyToTexture( TextureInterface* _target );

        /// D3D-specific member that returns the underlying surface.
        LPDIRECTDRAWSURFACE7 getDDSurface(void);

		/// @copydoc Texture::getBuffer
		HardwarePixelBufferInterface* getBuffer( size_t _idx ) override;
		void loadImpl() override;
		void createInternalResourcesImpl( ETextureType _type, int _usage, bool _hasAlpha,
			std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum ) override;
		void freeInternalResourcesImpl() override;
		std::size_t getWidth() override;
		std::size_t getHeight() override;
		/// Static pixelformat functions
		static D3DX_SURFACEFORMAT OgreFormat_to_D3DXFormat( PixelFormat format );
		static PixelFormat closestD3DXFormat( PixelFormat format );
		static bool OgreFormat_to_DDPixelFormat( PixelFormat format, DDPIXELFORMAT & out );

		int getPixelFormat();
		std::size_t getNumFaces() const;
		/// Restore this texture from a lost device
		//void restoreFromLostDevice(void);
		HardwarePixelBufferInterface* createHardwarePixelBuffer( EHardwareBufferUsage _usage ) override;

    protected:
        IDirect3DDevice7 * m_D3DDevice;       ///< A pointer to the Direct3D device.
        IDirectDrawSurface7 * m_surface;      ///< Surface of the (first) device-specific texture.
		/// cube texture individual face names
		//String m_cubeFaceNames[6];
		std::size_t m_width;
		std::size_t m_height;
		/// Vector of pointers to subsurfaces
		//typedef std::vector<HardwarePixelBufferInterface*> TSurfaceList;
		//TSurfaceList	m_surfaceList;

		ETextureType m_type;
		PixelFormat m_format;
		bool m_hasAlpha;
		int m_usage;
		/// Are we restoring from a lost device?
		//bool m_restoring;
		
		void createSurface2D( std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum );
		void createSurface3D( std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum );

		/// internal method, return a D3D pixel format for texture creation
		void chooseD3DFormat_( DDPIXELFORMAT &ddpf );

		/// internal method, construct full cube texture face names from a given string
		//void constructCubeFaceNames_( const String& _name );

		/// internal method, the cube map face name for the spec. face index
		//const String& getCubeFaceName_( unsigned char _face ) const
		//{ assert( _face < 6 ); return m_cubeFaceNames[_face]; }

		// Create the list of surfaces
		void createSurfaceList_( std::size_t _mipmapsNum );
    };

    //Specialisation of SharedPtr to allow SharedPtr to be assigned to D3DTexturePtr 
    //@note Has to be a subclass since we need operator=.
    //We could templatise this instead of repeating per Resource subclass, 
    //except to do so requires a form VC6 does not support i.e.
    //ResourceSubclassPtr<T> : public SharedPtr<T>
    /*class D3DTexturePtr : public SharedPtr<D3DTexture> 
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
    };*/


    /// D3D7 implementation of RenderTexture
    /*class D3D7RenderTexture : public RenderTexture
    {
    public:
        D3D7RenderTexture( const String &name, D3D7HardwarePixelBuffer *buffer )
			: RenderTexture( buffer, 0 )
        {
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
			else if( name == "BUFFER" )
			{
				*static_cast<HardwarePixelBuffer**>(pData) = mBuffer;
				return;
			}
        }*/
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
    //};
}
