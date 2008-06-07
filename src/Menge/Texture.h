#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/TextureInterface.h"

#	include "HardwareBuffer.h"
#	include "Image.h"

namespace Menge
{
	typedef std::vector<const Image*> TConstImageVector;

	/// Enum identifying the loading state of the resource
	enum ELoadingState
	{
		/// Not loaded
		LOADSTATE_UNLOADED,
		/// Loading is in progress
		LOADSTATE_LOADING,
		/// Fully loaded
		LOADSTATE_LOADED,
		/// Currently unloading
		LOADSTATE_UNLOADING
	};

	class HardwarePixelBuffer;

	// Abstract class representing a Texture resource.
	// @remarks
	// The actual concrete subclass which will exist for a texture
	// is dependent on the rendering system in use (Direct3D, OpenGL etc).
	// This class represents the commonalities, and is the one 'used'
	// by programmers even though the real implementation could be
	// different in reality. Texture objects are created through
	// the 'create' method of the TextureManager concrete subclass.
	class Texture //: public Resource
	{
	public:
		Texture( const String& _name );
		~Texture();
		const String& getName() const 
		{ 
			return m_name; 
		}
		// Sets the type of texture; can only be changed before load() 
		void setTextureType( ETextureType ttype ) { m_textureType = ttype; }

		// Gets the type of texture 
		ETextureType getTextureType() const { return m_textureType; }

		// Gets the number of mipmaps to be used for this texture.
		std::size_t getNumMipmaps() const { return m_numMipmaps; }

		// Sets the number of mipmaps to be used for this texture.
		// @note
		// Must be set before calling any 'load' method.
		void setNumMipmaps( std::size_t num ) { m_numRequestedMipmaps = m_numMipmaps = num; }

		// Are mipmaps hardware generated?
		// @remarks
		// Will only be accurate after texture load, or createInternalResources
		bool getMipmapsHardwareGenerated() const { return m_mipmapsHardwareGenerated; }

		// Returns the gamma adjustment factor applied to this texture.
		float getGamma() const { return m_gamma; }

		// Sets the gamma adjustment factor applied to this texture.
		// @note
		// Must be called before any 'load' method.
		void setGamma( float _g ) { m_gamma = _g; }

		// Returns the height of the texture.
		std::size_t getHeight() const { return m_height; }

		// Returns the width of the texture.
		std::size_t getWidth() const { return m_width; }

		// Returns the depth of the texture (only applicable for 3D textures).
		std::size_t getDepth() const { return m_depth; }

		// Returns the height of the original input texture (may differ due to hardware requirements).
		std::size_t getSrcHeight() const { return m_srcHeight; }

		// Returns the width of the original input texture (may differ due to hardware requirements).
		std::size_t getSrcWidth() const { return m_srcWidth; }

		// Returns the original depth of the input texture (only applicable for 3D textures).
		std::size_t getSrcDepth() const { return m_srcDepth; }

		// Set the height of the texture; can only do this before load();
		void setHeight( std::size_t _h ) { m_height = m_srcHeight = _h; }

		// Set the width of the texture; can only do this before load();
		void setWidth( std::size_t _w ) { m_width = m_srcWidth = _w; }

		// Set the depth of the texture (only applicable for 3D textures);
		// can only do this before load();
		void setDepth( std::size_t _d )  { m_depth = m_srcDepth = _d; }

		// Returns the TextureUsage indentifier for this Texture
		int getUsage() const
		{
			return m_usage;
		}

		// Sets the TextureUsage indentifier for this Texture; only useful before load()

		// @param u is a combination of TU_STATIC, TU_DYNAMIC, TU_WRITE_ONLY 
		// TU_AUTOMIPMAP and TU_RENDERTARGET (see TextureUsage enum). You are
		// strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to 
		// update regularly, consider HBU_DYNAMIC_WRITE_ONLY.
		void setUsage( int _u ) { m_usage = _u; }

		//Creates the internal texture resources for this texture. 
		//@remarks
		//This method creates the internal texture resources (pixel buffers, 
		//texture surfaces etc) required to begin using this texture. You do
		//not need to call this method directly unless you are manually creating
		//a texture, in which case something must call it, after having set the
		//size and format of the texture (e.g. the ManualResourceLoader might
		//be the best one to call it). If you are not defining a manual texture,
		//or if you use one of the self-contained load...() methods, then it will be
		//called for you.
		void createInternalResources();

		// Frees internal texture resources for this texture.
		void freeInternalResources();

		// Copies (and maybe scales to fit) the contents of this texture to
		// another texture.
		void copyToTexture( Texture* _target );

		// Loads the data from an image.
		// @note Important: only call this from outside the load() routine of a 
		// Resource. Don't call it within (including ManualResourceLoader) - use
		// _loadImages() instead. This method is designed to be external, 
		// performs locking and checks the load status before loading.
		void loadImage( const Image& _img );

		void loadImage3D( const Image _imgs[] );

		// Loads the data from a raw stream.
		// @note Important: only call this from outside the load() routine of a 
		// Resource. Don't call it within (including ManualResourceLoader) - use
		// _loadImages() instead. This method is designed to be external, 
		// performs locking and checks the load status before loading.
		// @param stream Data stream containing the raw pixel data
		// @param uWidth Width of the image
		// @param uHeight Height of the image
		// @param eFormat The format of the pixel data
		void loadRawData( DataStreamInterface* _stream, 
			unsigned short _width, unsigned short _height, PixelFormat _format );

		// Internal method to load the texture from a set of images. 
		// @note Do NOT call this method unless you are inside the load() routine
		// already, e.g. a ManualResourceLoader. It is not threadsafe and does
		// not check or update resource loading status.
		void loadImages_( const TConstImageVector& _images );

		// Returns the pixel format for the texture surface.
		PixelFormat getFormat() const
		{
			return m_format;
		}

		// Returns the desired pixel format for the texture surface.
		PixelFormat getDesiredFormat() const
		{
			return m_desiredFormat;
		}

		// Returns the pixel format of the original input texture (may differ due to
		// hardware requirements and pixel format convertion).
		PixelFormat getSrcFormat() const
		{
			return m_srcFormat;
		}

		// Sets the pixel format for the texture surface; can only be set before load().
		void setFormat( PixelFormat _pf );

		// Returns true if the texture has an alpha layer.
		bool hasAlpha() const;

		// Sets desired bit depth for integer pixel format textures.
		// @note
		// Available values: 0, 16 and 32, where 0 (the default) means keep original format
		// as it is. This value is number of bits for the pixel.
		void setDesiredIntegerBitDepth( unsigned short _bits );

		// gets desired bit depth for integer pixel format textures.
		unsigned short getDesiredIntegerBitDepth() const;

		// Sets desired bit depth for float pixel format textures.
		// @note
		// Available values: 0, 16 and 32, where 0 (the default) means keep original format
		// as it is. This value is number of bits for a channel of the pixel.
		void setDesiredFloatBitDepth( unsigned short _bits );

		// gets desired bit depth for float pixel format textures.
		unsigned short getDesiredFloatBitDepth() const;

		// Sets desired bit depth for integer and float pixel format.
		void setDesiredBitDepths( unsigned short _integerBits, unsigned short _floatBits );

		// Sets whether luminace pixel format will treated as alpha format when load this texture.
		void setTreatLuminanceAsAlpha( bool _asAlpha );

		// Gets whether luminace pixel format will treated as alpha format when load this texture.
		bool getTreatLuminanceAsAlpha() const;

		// Return the number of faces this texture has. This will be 6 for a cubemap
		// texture and 1 for a 1D, 2D or 3D one.
		std::size_t getNumFaces() const;

		//Return hardware pixel buffer for a surface. This buffer can then
		//be used to copy data from and to a particular level of the texture.
		//@param face 	Face number, in case of a cubemap texture. Must be 0
		//for other types of textures.
		//For cubemaps, this is one of 
		//+X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
		//@param mipmap	Mipmap level. This goes from 0 for the first, largest
		//mipmap level to getNumMipmaps()-1 for the smallest.
		//@returns	A shared pointer to a hardware pixel buffer
		//@remarks	The buffer is invalidated when the resource is unloaded or destroyed.
		//Do not use it after the lifetime of the containing texture.
		HardwarePixelBuffer* getBuffer( std::size_t face = 0, std::size_t _mipmap = 0 );

		void restoreFromLostDevice();

		// Internal implementation of the meat of the 'load' action, only called if this 
		// resource is not being loaded from a ManualResourceLoader. 
		void loadImpl();

		// Internal implementation of the 'unload' action; called regardless of
		// whether this resource is being loaded from a ManualResourceLoader. 
		void unloadImpl();

		void load();
		void unload();

		void constructCubeFaceNames_( const String& _name );

	protected:
		String m_name;
		/// Is the resource currently loaded?
		volatile ELoadingState m_loadingState;
		std::size_t m_height;
		std::size_t m_width;
		std::size_t m_depth;

		/// The size of the resource in bytes
		size_t m_size;
		std::size_t m_numRequestedMipmaps;
		std::size_t m_numMipmaps;
		bool m_mipmapsHardwareGenerated;
		float m_gamma;

		ETextureType m_textureType;
		PixelFormat m_format;
		int m_usage; // Bit field, so this can't be TextureUsage

		PixelFormat m_srcFormat;
		std::size_t m_srcWidth, m_srcHeight, m_srcDepth;

		PixelFormat m_desiredFormat;
		unsigned short m_desiredIntegerBitDepth;
		unsigned short m_desiredFloatBitDepth;
		bool m_treatLuminanceAsAlpha;

		bool m_internalResourcesCreated;

		/// @copydoc Resource::calculateSize
		std::size_t calculateSize_() const;

		// Implementation of creating internal texture resources 
		virtual void createInternalResourcesImpl(void) = 0;

		// Implementation of freeing internal texture resources 
		virtual void freeInternalResourcesImpl(void) = 0;

		// Default implementation of unload which calls freeInternalResources
		//void unloadImpl();

		// Hardware Implementation Interface
		TextureInterface*	m_interface;

		String m_cubeFaceNames[6];

		/// Are we restoring from a lost device?
		bool m_restoring;

		typedef std::vector<HardwarePixelBuffer*> TSurfaceList;
		TSurfaceList m_surfaceList;
	};

}	// namespace Menge
