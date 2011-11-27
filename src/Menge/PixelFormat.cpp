
#	include "PixelFormat.h"
#	include "Bitwise.h"

#	include <algorithm>
#	include <map>

#	include <memory.h>

namespace Menge
{
	// A record that describes a pixel format in detail.
	struct PixelFormatDescription 
	{
		// Name of the format, as in the enum 
		TChar *name;
		// Number of bytes one element (colour value) takes.
		unsigned char elemBytes;
		// Pixel format flags, see enum PixelFormatFlags for the bit field
		// definitions
		uint32 flags;
		// Component type
		PixelComponentType componentType;
		// Component count
		unsigned char componentCount;
		// Number of bits for red(or luminance), green, blue, alpha
		unsigned char rbits,gbits,bbits,abits; // , ibits, dbits, ... 

		// Masks and shifts as used by packers/unpackers 
		uint32 rmask, gmask, bmask, amask;
		unsigned char rshift, gshift, bshift, ashift;
	};
	//////////////////////////////////////////////////////////////////////////
	// Pixel format database 
	//////////////////////////////////////////////////////////////////////////
	PixelFormatDescription _pixelFormats[PF_COUNT] = {
		{"PF_UNKNOWN",
		/* Bytes per element */
		0,
		/* Flags */
		0,
		/* Component type and count */
		PCT_BYTE, 0,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_L8",
		1,
		PFF_LUMINANCE | PFF_NATIVEENDIAN,
		PCT_BYTE, 1,
		8, 0, 0, 0,
		0xFF, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_L16",
		2,
		PFF_LUMINANCE | PFF_NATIVEENDIAN,
		PCT_SHORT, 1,
		16, 0, 0, 0,
		0xFFFF, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A8",
		1,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 1,
		0, 0, 0, 8,
		0, 0, 0, 0xFF, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A4L4",
		1,
		PFF_HASALPHA | PFF_LUMINANCE | PFF_NATIVEENDIAN,
		PCT_BYTE, 2,
		4, 0, 0, 4,
		0x0F, 0, 0, 0xF0, 0, 0, 0, 4
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_BYTE_LA",
		2,
		PFF_HASALPHA | PFF_LUMINANCE,
		PCT_BYTE, 2,
		8, 0, 0, 8,
		0,0,0,0,0,0,0,0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_R5G6B5",
		2,
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		5, 6, 5, 0,
		0xF800, 0x07E0, 0x001F, 0,
		11, 5, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_B5G6R5",
		2,
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		5, 6, 5, 0,
		0x001F, 0x07E0, 0xF800, 0,
		0, 5, 11, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A4R4G4B4",
		2,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		4, 4, 4, 4,
		0x0F00, 0x00F0, 0x000F, 0xF000,
		8, 4, 0, 12
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A1R5G5B5",
		2,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		5, 5, 5, 1,
		0x7C00, 0x03E0, 0x001F, 0x8000,
		10, 5, 0, 15,
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_R8G8B8",
		3,  // 24 bit integer -- special
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		8, 8, 8, 0,
		0xFF0000, 0x00FF00, 0x0000FF, 0,
		16, 8, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_B8G8R8",
		3,  // 24 bit integer -- special
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		8, 8, 8, 0,
		0x0000FF, 0x00FF00, 0xFF0000, 0,
		0, 8, 16, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A8R8G8B8",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		8, 8, 8, 8,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
		16, 8, 0, 24
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A8B8G8R8",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		8, 8, 8, 8,
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_B8G8R8A8",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		8, 8, 8, 8,
		0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF,
		8, 16, 24, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A2R10G10B10",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		10, 10, 10, 2,
		0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000,
		20, 10, 0, 30
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_A2B10G10R10",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		10, 10, 10, 2,
		0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
		0, 10, 20, 30
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DXT1",
		0,
		PFF_COMPRESSED | PFF_HASALPHA,
		PCT_BYTE, 3, // No alpha
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DXT2",
		0,
		PFF_COMPRESSED | PFF_HASALPHA,
		PCT_BYTE, 4,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DXT3",
		0,
		PFF_COMPRESSED | PFF_HASALPHA,
		PCT_BYTE, 4,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DXT4",
		0,
		PFF_COMPRESSED | PFF_HASALPHA,
		PCT_BYTE, 4,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DXT5",
		0,
		PFF_COMPRESSED | PFF_HASALPHA,
		PCT_BYTE, 4,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT16_RGB",
		6,
		PFF_FLOAT,
		PCT_FLOAT16, 3,
		16, 16, 16, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT16_RGBA",
		8,
		PFF_FLOAT,
		PCT_FLOAT16, 4,
		16, 16, 16, 16,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT32_RGB",
		12,
		PFF_FLOAT,
		PCT_FLOAT32, 3,
		32, 32, 32, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT32_RGBA",
		16,
		PFF_FLOAT,
		PCT_FLOAT32, 4,
		32, 32, 32, 32,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_X8R8G8B8",
		4,
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		8, 8, 8, 0,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
		16, 8, 0, 24
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_X8B8G8R8",
		4,
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		8, 8, 8, 0,
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_R8G8B8A8",
		4,
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		PCT_BYTE, 4,
		8, 8, 8, 8,
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
		24, 16, 8, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_DEPTH",
		4,
		PFF_DEPTH,
		PCT_FLOAT32, 1, // ?
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_SHORT_RGBA",
		8,
		PFF_HASALPHA,
		PCT_SHORT, 4,
		16, 16, 16, 16,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_R3G3B2",
		1,
		PFF_NATIVEENDIAN,
		PCT_BYTE, 3,
		3, 3, 2, 0,
		0xE0, 0x1C, 0x03, 0,
		5, 2, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT16_R",
		2,
		PFF_FLOAT,
		PCT_FLOAT16, 1,
		16, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT32_R",
		4,
		PFF_FLOAT,
		PCT_FLOAT32, 1,
		32, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_SHORT_GR",
		4,
		PFF_NATIVEENDIAN,
		PCT_SHORT, 2,
		16, 16, 0, 0,
		0x0000FFFF, 0xFFFF0000, 0, 0, 
		0, 16, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT16_GR",
		4,
		PFF_FLOAT,
		PCT_FLOAT16, 2,
		16, 16, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_FLOAT32_GR",
		8,
		PFF_FLOAT,
		PCT_FLOAT32, 2,
		32, 32, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//////////////////////////////////////////////////////////////////////////
		{"PF_SHORT_RGB",
		6,
		0,
		PCT_SHORT, 3,
		16, 16, 16, 0,
		0, 0, 0, 0, 0, 0, 0, 0
		},
	};
	//////////////////////////////////////////////////////////////////////////
	std::size_t PixelBox::getConsecutiveSize() const
	{
		return PixelUtil::getMemorySize( getWidth(), getHeight(), getDepth(), format );
	}
	//////////////////////////////////////////////////////////////////////////
	/*PixelBox PixelBox::getSubVolume(const Box &def) const
	{
		if(PixelUtil::isCompressed(format))
		{
			if(def.left == left && def.top == top && def.front == front &&
				def.right == right && def.bottom == bottom && def.back == back)
			{
				// Entire buffer is being queried
				return *this;
			}
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Cannot return subvolume of compressed PixelBuffer", "PixelBox::getSubVolume");
		}
		if(!contains(def))
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Bounds out of range", "PixelBox::getSubVolume");

		const size_t elemSize = PixelUtil::getNumElemBytes(format);
		// Calculate new data origin
		PixelBox rval(def, format, ((uint8*)data)
			+ ((def.left-left)*elemSize)
			+ ((def.top-top)*rowPitch*elemSize)
			+ ((def.front-front)*slicePitch*elemSize)
			);

		rval.rowPitch = rowPitch;
		rval.slicePitch = slicePitch;
		rval.format = format;

		return rval;
	}*/
	//////////////////////////////////////////////////////////////////////////
	static inline const PixelFormatDescription &getDescriptionFor( const PixelFormat _fmt )
	{
		const int ord = (int)_fmt;
		assert(ord>=0 && ord<PF_COUNT);

		return _pixelFormats[ord];
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t PixelUtil::getNumElemBytes( PixelFormat _format )
	{
		return getDescriptionFor( _format ).elemBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t PixelUtil::getMemorySize( std::size_t _width, std::size_t _height, std::size_t _depth, PixelFormat _format )
	{
		if( isCompressed( _format ) )
		{
			switch( _format )
			{
				// DXT formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes. DXT can only be used on 2D images.
			case PF_DXT1:
				assert( _depth == 1 );
				return ( (_width+3)/4)*((_height+3)/4 )*8;
			case PF_DXT2:
			case PF_DXT3:
			case PF_DXT4:
			case PF_DXT5:
				assert( _depth == 1 );
				return ((_width+3)/4)*((_height+3)/4)*16;
			default:
				assert( 0 && "PixelUtil::getMemorySize -> Invalid compressed pixel format" );
			}
		}
		else
		{
			return _width*_height*_depth*getNumElemBytes( _format );
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PixelUtil::getNumElemBits( PixelFormat _format )
	{
		return getDescriptionFor(_format).elemBytes * 8;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int PixelUtil::getFlags( PixelFormat _format )
	{
		return getDescriptionFor(_format).flags;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::hasAlpha(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_HASALPHA) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isFloatingPoint(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_FLOAT) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isCompressed(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_COMPRESSED) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isDepth(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_DEPTH) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isNativeEndian(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_NATIVEENDIAN) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isLuminance(PixelFormat _format)
	{
		return (PixelUtil::getFlags(_format) & PFF_LUMINANCE) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isValidExtent( std::size_t _width, std::size_t _height, std::size_t _depth, PixelFormat _format )
	{
		if(isCompressed(_format))
		{
			switch(_format)
			{
			case PF_DXT1:
			case PF_DXT2:
			case PF_DXT3:
			case PF_DXT4:
			case PF_DXT5:
				return ((_width&3)==0 && (_height&3)==0 && _depth==1);
			default:
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::getBitDepths( PixelFormat _format, int _rgba[4] )
	{
		const PixelFormatDescription &des = getDescriptionFor(_format);
		_rgba[0] = des.rbits;
		_rgba[1] = des.gbits;
		_rgba[2] = des.bbits;
		_rgba[3] = des.abits;
	}
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::getBitMasks( PixelFormat _format, uint32 _rgba[4] )
	{
		const PixelFormatDescription &des = getDescriptionFor(_format);
		_rgba[0] = des.rmask;
		_rgba[1] = des.gmask;
		_rgba[2] = des.bmask;
		_rgba[3] = des.amask;
	}
	//////////////////////////////////////////////////////////////////////////
	String PixelUtil::getFormatName( PixelFormat _srcformat )
	{
		return getDescriptionFor( _srcformat ).name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PixelUtil::isAccessible(PixelFormat _srcformat)
	{
		if (_srcformat == PF_UNKNOWN)
			return false;
		unsigned int flags = getFlags(_srcformat);
		return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
	}
	//////////////////////////////////////////////////////////////////////////
	PixelComponentType PixelUtil::getComponentType(PixelFormat _fmt)
	{
		const PixelFormatDescription &des = getDescriptionFor(_fmt);
		return des.componentType;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PixelUtil::getComponentCount(PixelFormat _fmt)
	{
		const PixelFormatDescription &des = getDescriptionFor(_fmt);
		return des.componentCount;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat PixelUtil::getFormatFromName(const String& _name, bool _accessibleOnly, bool _caseSensitive)
	{
		String tmp = _name;
		if (!_caseSensitive)
		{
			// We are stored upper-case format names.
			std::transform(
				tmp.begin(),
				tmp.end(),
				tmp.begin(),
				toupper);
		}

		for (int i = 0; i < PF_COUNT; ++i)
		{
			PixelFormat pf = static_cast<PixelFormat>(i);
			if (!_accessibleOnly || isAccessible(pf))
			{
				if (tmp == getFormatName(pf))
					return pf;
			}
		}
		return PF_UNKNOWN;
	}
	//////////////////////////////////////////////////////////////////////////
	String PixelUtil::getBNFExpressionOfPixelFormats(bool _accessibleOnly)
	{
		// Collect format names sorted by length, it's required by BNF compiler
		// that similar tokens need longer ones comes first.
		typedef std::multimap<String::size_type, String> FormatNameMap;
		FormatNameMap formatNames;
		for (size_t i = 0; i < PF_COUNT; ++i)
		{
			PixelFormat pf = static_cast<PixelFormat>(i);
			if (!_accessibleOnly || isAccessible(pf))
			{
				String formatName = getFormatName(pf);
				formatNames.insert(std::make_pair(formatName.length(), formatName));
			}
		}

		// Populate the BNF expression in reverse order
		String result;
		// Note: Stupid M$ VC7.1 can't dealing operator!= with FormatNameMap::const_reverse_iterator.
		for (FormatNameMap::reverse_iterator j = formatNames.rbegin(); j != formatNames.rend(); ++j)
		{
			if (!result.empty())
				result += " | ";
			result += "'" + j->second + "'";
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat PixelUtil::getFormatForBitDepths(PixelFormat _fmt, unsigned short _integerBits, unsigned short _floatBits)
	{
		switch (_integerBits)
		{
		case 16:
			switch (_fmt)
			{
			case PF_R8G8B8:
			case PF_X8R8G8B8:
				return PF_R5G6B5;

			case PF_B8G8R8:
			case PF_X8B8G8R8:
				return PF_B5G6R5;

			case PF_A8R8G8B8:
			case PF_R8G8B8A8:
			case PF_A8B8G8R8:
			case PF_B8G8R8A8:
				return PF_A4R4G4B4;

			case PF_A2R10G10B10:
			case PF_A2B10G10R10:
				return PF_A1R5G5B5;

			default:
				// use original image format
				break;
			}
			break;

		case 32:
			switch (_fmt)
			{
			case PF_R5G6B5:
				return PF_X8R8G8B8;

			case PF_B5G6R5:
				return PF_X8B8G8R8;

			case PF_A4R4G4B4:
				return PF_A8R8G8B8;

			case PF_A1R5G5B5:
				return PF_A2R10G10B10;

			default:
				// use original image format
				break;
			}
			break;

		default:
			// use original image format
			break;
		}

		switch (_floatBits)
		{
		case 16:
			switch (_fmt)
			{
			case PF_FLOAT32_R:
				return PF_FLOAT16_R;

			case PF_FLOAT32_RGB:
				return PF_FLOAT16_RGB;

			case PF_FLOAT32_RGBA:
				return PF_FLOAT16_RGBA;

			default:
				// use original image format
				break;
			}
			break;

		case 32:
			switch (_fmt)
			{
			case PF_FLOAT16_R:
				return PF_FLOAT32_R;

			case PF_FLOAT16_RGB:
				return PF_FLOAT32_RGB;

			case PF_FLOAT16_RGBA:
				return PF_FLOAT32_RGBA;

			default:
				// use original image format
				break;
			}
			break;

		default:
			// use original image format
			break;
		}

		return _fmt;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Pixel packing/unpacking utilities
	//////////////////////////////////////////////////////////////////////////
	/*void PixelUtil::packColour( const ColourValue& _colour, const PixelFormat _pf,  void* _dest )
	{
		packColour( _colour.getR(), _colour.getG(), _colour.getB(), _colour.getA(), _pf, _dest);
	}*/
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::packColour( const uint8 _r, const uint8 _g, const uint8 _b, const uint8 _a, const PixelFormat _pf,  void* _dest )
	{
		const PixelFormatDescription &des = getDescriptionFor(_pf);
		if(des.flags & PFF_NATIVEENDIAN) 
		{
			// Shortcut for integer formats packing
			unsigned int value = ((Bitwise::fixedToFixed(_r, 8, des.rbits)<<des.rshift) & des.rmask) |
				((Bitwise::fixedToFixed(_g, 8, des.gbits)<<des.gshift) & des.gmask) |
				((Bitwise::fixedToFixed(_b, 8, des.bbits)<<des.bshift) & des.bmask) |
				((Bitwise::fixedToFixed(_a, 8, des.abits)<<des.ashift) & des.amask);
			// And write to memory
			Bitwise::intWrite(_dest, des.elemBytes, value);
		} 
		else 
		{
			// Convert to float
			packColour((float)_r/255.0f,(float)_g/255.0f,(float)_b/255.0f,(float)_a/255.0f, _pf, _dest);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::packColour( const float _r, const float _g, const float _b, const float _a, const PixelFormat _pf,  void* _dest )
	{
		// Catch-it-all here
		const PixelFormatDescription &des = getDescriptionFor(_pf);
		if(des.flags & PFF_NATIVEENDIAN) 
		{
			// Do the packing
			//std::cerr << dest << " " << r << " " << g <<  " " << b << " " << a << std::endl;
			const unsigned int value = ((Bitwise::floatToFixed(_r, des.rbits)<<des.rshift) & des.rmask) |
				((Bitwise::floatToFixed(_g, des.gbits)<<des.gshift) & des.gmask) |
				((Bitwise::floatToFixed(_b, des.bbits)<<des.bshift) & des.bmask) |
				((Bitwise::floatToFixed(_a, des.abits)<<des.ashift) & des.amask);
			// And write to memory
			Bitwise::intWrite(_dest, des.elemBytes, value);
		} 
		else 
		{
			switch(_pf)
			{
			case PF_FLOAT32_R:
				((float*)_dest)[0] = _r;
				break;
			case PF_FLOAT32_GR:
				((float*)_dest)[0] = _g;
				((float*)_dest)[1] = _r;
				break;
			case PF_FLOAT32_RGB:
				((float*)_dest)[0] = _r;
				((float*)_dest)[1] = _g;
				((float*)_dest)[2] = _b;
				break;
			case PF_FLOAT32_RGBA:
				((float*)_dest)[0] = _r;
				((float*)_dest)[1] = _g;
				((float*)_dest)[2] = _b;
				((float*)_dest)[3] = _a;
				break;
			case PF_FLOAT16_R:
				((uint16*)_dest)[0] = Bitwise::floatToHalf(_r);
				break;
			case PF_FLOAT16_GR:
				((uint16*)_dest)[0] = Bitwise::floatToHalf(_g);
				((uint16*)_dest)[1] = Bitwise::floatToHalf(_r);
				break;
			case PF_FLOAT16_RGB:
				((uint16*)_dest)[0] = Bitwise::floatToHalf(_r);
				((uint16*)_dest)[1] = Bitwise::floatToHalf(_g);
				((uint16*)_dest)[2] = Bitwise::floatToHalf(_b);
				break;
			case PF_FLOAT16_RGBA:
				((uint16*)_dest)[0] = Bitwise::floatToHalf(_r);
				((uint16*)_dest)[1] = Bitwise::floatToHalf(_g);
				((uint16*)_dest)[2] = Bitwise::floatToHalf(_b);
				((uint16*)_dest)[3] = Bitwise::floatToHalf(_a);
				break;
			case PF_SHORT_RGB:
				((uint16*)_dest)[0] = Bitwise::floatToFixed(_r, 16);
				((uint16*)_dest)[1] = Bitwise::floatToFixed(_g, 16);
				((uint16*)_dest)[2] = Bitwise::floatToFixed(_b, 16);
				break;
			case PF_SHORT_RGBA:
				((uint16*)_dest)[0] = Bitwise::floatToFixed(_r, 16);
				((uint16*)_dest)[1] = Bitwise::floatToFixed(_g, 16);
				((uint16*)_dest)[2] = Bitwise::floatToFixed(_b, 16);
				((uint16*)_dest)[3] = Bitwise::floatToFixed(_a, 16);
				break;
			case PF_BYTE_LA:
				((uint8*)_dest)[0] = Bitwise::floatToFixed(_r, 8);
				((uint8*)_dest)[1] = Bitwise::floatToFixed(_a, 8);
				break;
			default:
				// Not yet supported
				assert( 0 && String( "PixelUtil::packColour -> pack to "+getFormatName(_pf)+" not implemented" ).c_str() );
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/*void PixelUtil::unpackColour( ColourValue* _colour, PixelFormat _pf,  const void* _src )
	{
		unpackColour(&_colour->r, &_colour->g, &_colour->b, &_colour->a, _pf, _src );
	}*/
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::unpackColour( uint8 *_r, uint8 *_g, uint8 *_b, uint8 *_a, PixelFormat _pf,  const void* _src )
	{
		const PixelFormatDescription &des = getDescriptionFor(_pf);
		if(des.flags & PFF_NATIVEENDIAN) {
			// Shortcut for integer formats unpacking
			const unsigned int value = Bitwise::intRead(_src, des.elemBytes);
			if(des.flags & PFF_LUMINANCE)
			{
				// Luminance format -- only rbits used
				*_r = *_g = *_b = Bitwise::fixedToFixed(
					(value & des.rmask)>>des.rshift, des.rbits, 8);
			}
			else
			{
				*_r = Bitwise::fixedToFixed((value & des.rmask)>>des.rshift, des.rbits, 8);
				*_g = Bitwise::fixedToFixed((value & des.gmask)>>des.gshift, des.gbits, 8);
				*_b = Bitwise::fixedToFixed((value & des.bmask)>>des.bshift, des.bbits, 8);
			}
			if(des.flags & PFF_HASALPHA)
			{
				*_a = Bitwise::fixedToFixed((value & des.amask)>>des.ashift, des.abits, 8);
			}
			else
			{
				*_a = 255; // No alpha, default a component to full
			}
		} else {
			// Do the operation with the more generic floating point
			float rr, gg, bb, aa;
			unpackColour(&rr,&gg,&bb,&aa, _pf, _src);
			*_r = Bitwise::floatToFixed(rr, 8);
			*_g = Bitwise::floatToFixed(gg, 8);
			*_b = Bitwise::floatToFixed(bb, 8);
			*_a = Bitwise::floatToFixed(aa, 8);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::unpackColour( float *r, float *g, float *b, float *a, PixelFormat _pf,  const void* _src )
	{
		const PixelFormatDescription &des = getDescriptionFor(_pf);
		if(des.flags & PFF_NATIVEENDIAN) {
			// Shortcut for integer formats unpacking
			const unsigned int value = Bitwise::intRead(_src, des.elemBytes);
			if(des.flags & PFF_LUMINANCE)
			{
				// Luminance format -- only rbits used
				*r = *g = *b = Bitwise::fixedToFloat(
					(value & des.rmask)>>des.rshift, des.rbits);
			}
			else
			{
				*r = Bitwise::fixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
				*g = Bitwise::fixedToFloat((value & des.gmask)>>des.gshift, des.gbits);
				*b = Bitwise::fixedToFloat((value & des.bmask)>>des.bshift, des.bbits);
			}
			if(des.flags & PFF_HASALPHA)
			{
				*a = Bitwise::fixedToFloat((value & des.amask)>>des.ashift, des.abits);
			}
			else
			{
				*a = 1.0f; // No alpha, default a component to full
			}
		} else {
			switch(_pf)
			{
			case PF_FLOAT32_R:
				*r = *g = *b = ((float*)_src)[0];
				*a = 1.0f;
				break;
			case PF_FLOAT32_GR:
				*g = ((float*)_src)[0];
				*r = *b = ((float*)_src)[1];
				*a = 1.0f;
				break;
			case PF_FLOAT32_RGB:
				*r = ((float*)_src)[0];
				*g = ((float*)_src)[1];
				*b = ((float*)_src)[2];
				*a = 1.0f;
				break;
			case PF_FLOAT32_RGBA:
				*r = ((float*)_src)[0];
				*g = ((float*)_src)[1];
				*b = ((float*)_src)[2];
				*a = ((float*)_src)[3];
				break;
			case PF_FLOAT16_R:
				*r = *g = *b = Bitwise::halfToFloat(((uint16*)_src)[0]);
				*a = 1.0f;
				break;
			case PF_FLOAT16_GR:
				*g = Bitwise::halfToFloat(((uint16*)_src)[0]);
				*r = *b = Bitwise::halfToFloat(((uint16*)_src)[1]);
				*a = 1.0f;
				break;
			case PF_FLOAT16_RGB:
				*r = Bitwise::halfToFloat(((uint16*)_src)[0]);
				*g = Bitwise::halfToFloat(((uint16*)_src)[1]);
				*b = Bitwise::halfToFloat(((uint16*)_src)[2]);
				*a = 1.0f;
				break;
			case PF_FLOAT16_RGBA:
				*r = Bitwise::halfToFloat(((uint16*)_src)[0]);
				*g = Bitwise::halfToFloat(((uint16*)_src)[1]);
				*b = Bitwise::halfToFloat(((uint16*)_src)[2]);
				*a = Bitwise::halfToFloat(((uint16*)_src)[3]);
				break;
			case PF_SHORT_RGB:
				*r = Bitwise::fixedToFloat(((uint16*)_src)[0], 16);
				*g = Bitwise::fixedToFloat(((uint16*)_src)[1], 16);
				*b = Bitwise::fixedToFloat(((uint16*)_src)[2], 16);
				*a = 1.0f;
				break;
			case PF_SHORT_RGBA:
				*r = Bitwise::fixedToFloat(((uint16*)_src)[0], 16);
				*g = Bitwise::fixedToFloat(((uint16*)_src)[1], 16);
				*b = Bitwise::fixedToFloat(((uint16*)_src)[2], 16);
				*a = Bitwise::fixedToFloat(((uint16*)_src)[3], 16);
				break;
			case PF_BYTE_LA:
				*r = *g = *b = Bitwise::fixedToFloat(((uint8*)_src)[0], 8);
				*a = Bitwise::fixedToFloat(((uint8*)_src)[1], 8);
				break;
			default:
				// Not yet supported
				assert( 0 && String( "PixelUtil::unpackColour -> unpack from "+getFormatName(_pf)+" not implemented" ).c_str() );
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/* Convert pixels from one format to another */
	void PixelUtil::bulkPixelConversion( void* _srcp, PixelFormat _srcFormat,
		void* _destp, PixelFormat _dstFormat, unsigned int _count )
	{
		PixelBox src( _count, 1, 1, _srcFormat, _srcp ),
			dst( _count, 1, 1, _dstFormat, _destp );

		bulkPixelConversion( src, dst );
	}
	//////////////////////////////////////////////////////////////////////////
	void PixelUtil::bulkPixelConversion(const PixelBox &src, const PixelBox &dst)
	{
		assert(src.getWidth() == dst.getWidth() &&
			src.getHeight() == dst.getHeight() &&
			src.getDepth() == dst.getDepth());

		// Check for compressed formats, we don't support decompression, compression or recoding
		if(PixelUtil::isCompressed(src.format) || PixelUtil::isCompressed(dst.format))
		{
			if(src.format == dst.format)
			{
				memcpy(dst.data, src.data, src.getConsecutiveSize());
				return;
			}
			else
			{
				assert( 0 && "PixelUtil::bulkPixelConversion -> This method can not be used to compress or decompress images" );
			}
		}

		// The easy case
		if(src.format == dst.format) 
		{
			// Everything consecutive?
			if(src.isConsecutive() && dst.isConsecutive())
			{
				memcpy(dst.data, src.data, src.getConsecutiveSize());
				return;
			}

			uint8 *srcptr = static_cast<uint8*>(src.data);
			uint8 *dstptr = static_cast<uint8*>(dst.data);
			const size_t srcPixelSize = PixelUtil::getNumElemBytes(src.format);
			const size_t dstPixelSize = PixelUtil::getNumElemBytes(dst.format);

			// Calculate pitches+skips in bytes
			const size_t srcRowPitchBytes = src.rowPitch*srcPixelSize;
			//const size_t srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
			const size_t srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

			const size_t dstRowPitchBytes = dst.rowPitch*dstPixelSize;
			//const size_t dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
			const size_t dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

			// Otherwise, copy per row
			const size_t rowSize = src.getWidth()*srcPixelSize;
			for(size_t z=src.front; z<src.back; z++)
			{
				for(size_t y=src.top; y<src.bottom; y++)
				{
					memcpy(dstptr, srcptr, rowSize);
					srcptr += srcRowPitchBytes;
					dstptr += dstRowPitchBytes;
				}
				srcptr += srcSliceSkipBytes;
				dstptr += dstSliceSkipBytes;
			}
			return;
		}
		// Converting to PF_X8R8G8B8 is exactly the same as converting to
		// PF_A8R8G8B8. (same with PF_X8B8G8R8 and PF_A8B8G8R8)
		if(dst.format == PF_X8R8G8B8 || dst.format == PF_X8B8G8R8)
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of
			// optimized conversions
			PixelBox tempdst = dst;
			tempdst.format = dst.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(src, tempdst);
			return;
		}
		// Converting from PF_X8R8G8B8 is exactly the same as converting from
		// PF_A8R8G8B8, given that the destination format does not have alpha.
		if((src.format == PF_X8R8G8B8||src.format == PF_X8B8G8R8) && !hasAlpha(dst.format))
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of
			// optimized conversions
			PixelBox tempsrc = src;
			tempsrc.format = src.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(tempsrc, dst);
			return;
		}

		// NB VC6 can't handle the templates required for optimised conversion, tough
#if MENGE_COMPILER != MENGE_COMPILER_MSVC || MENGE_COMP_VER >= 1300
		// Is there a specialized, inlined, conversion?
		/*if(doOptimizedConversion(src, dst))
		{
			// If so, good
			return;
		}*/
#endif

		uint8 *srcptr = static_cast<uint8*>(src.data);
		uint8 *dstptr = static_cast<uint8*>(dst.data);
		const size_t srcPixelSize = PixelUtil::getNumElemBytes(src.format);
		const size_t dstPixelSize = PixelUtil::getNumElemBytes(dst.format);

		// Calculate pitches+skips in bytes
		const size_t srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
		const size_t srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;
		const size_t dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
		const size_t dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

		// The brute force fallback
		float r,g,b,a;
		for(size_t z=src.front; z<src.back; z++)
		{
			for(size_t y=src.top; y<src.bottom; y++)
			{
				for(size_t x=src.left; x<src.right; x++)
				{
					unpackColour(&r, &g, &b, &a, src.format, srcptr);
					packColour(r, g, b, a, dst.format, dstptr);
					srcptr += srcPixelSize;
					dstptr += dstPixelSize;
				}
				srcptr += srcRowSkipBytes;
				dstptr += dstRowSkipBytes;
			}
			srcptr += srcSliceSkipBytes;
			dstptr += dstSliceSkipBytes;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
