#	pragma once

#	include "Config/Typedef.h"

#	include "HardwareBufferInterface.h"

namespace Menge
{
	// Enum identifying the texture type
	enum ETextureType
	{
		/// 1D texture, used in combination with 1D texture coordinates
		TEX_TYPE_1D = 1,
		/// 2D texture, used in combination with 2D texture coordinates (default)
		TEX_TYPE_2D = 2,
		/// 3D volume texture, used in combination with 3D texture coordinates
		TEX_TYPE_3D = 3,
		/// 3D cube map, used in combination with 3D texture coordinates
		TEX_TYPE_CUBE_MAP = 4
	};

	// Enum identifying special mipmap numbers
	enum ETextureMipmap
	{
		/// Generate mipmaps up to 1x1
		MIP_UNLIMITED = 0x7FFFFFFF,
		/// Use TextureManager default
		MIP_DEFAULT = -1
	};

	// Enum identifying the texture usage
	enum ETextureUsage
	{
		/// @copydoc HardwareBuffer::Usage
		TU_STATIC = HBU_STATIC,
		TU_DYNAMIC = HBU_DYNAMIC,
		TU_WRITE_ONLY = HBU_WRITE_ONLY,
		TU_STATIC_WRITE_ONLY = HBU_STATIC_WRITE_ONLY, 
		TU_DYNAMIC_WRITE_ONLY = HBU_DYNAMIC_WRITE_ONLY,
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE = HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
		/// mipmaps will be automatically generated for this texture
		TU_AUTOMIPMAP = 0x100,
		/// this texture will be a render target, ie. used as a target for render to texture
		/// setting this flag will ignore all other texture usages except TU_AUTOMIPMAP
		TU_RENDERTARGET = 0x200,
		/// default to automatic mipmap generation static textures
		TU_DEFAULT = TU_AUTOMIPMAP | TU_STATIC_WRITE_ONLY
	};

	class RenderTextureInterface
	{
	public:
		virtual void loadImpl() = 0;
		virtual void createInternalResourcesImpl( ETextureType _type, int _usage, bool _hasAlpha,
			size_t _srcWidth, size_t _srcHeight, size_t _mipmapsNum ) = 0;
		virtual void freeInternalResourcesImpl() = 0;
		virtual HardwarePixelBufferInterface* getBuffer(  size_t _idx  ) = 0;
		virtual int getPixelFormat() = 0;
		virtual size_t getWidth() = 0;	// Hardware width
		virtual size_t getHeight() = 0; // Hardware height

		virtual HardwarePixelBufferInterface* createHardwarePixelBuffer( EHardwareBufferUsage _usage ) = 0;
	};

}	// namespace Menge