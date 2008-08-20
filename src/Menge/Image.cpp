
#	include "Image.h"
#	include "ImageCodec.h"
#	include "LogEngine.h"

#	include "ImageCodecPNG.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodec::~ImageCodec() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Image::Image()
	: m_width( 0 )
	, m_height( 0 )
	, m_depth( 0 )
	, m_size( 0 )
	, m_numMipmaps( 0 )
	, m_flags( 0 )
	, m_format( PF_UNKNOWN )
	, m_buffer( NULL )
	, m_autoDelete( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Image::Image( const Image& _img )
	: m_buffer( NULL )
	, m_autoDelete( true )
	{
		// call assignment operator
		*this = _img;
	}
	//////////////////////////////////////////////////////////////////////////
	Image::~Image()
	{
		//Only delete if this was not a dynamic image (meaning app holds & destroys buffer)
		if( m_buffer && m_autoDelete )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Image & Image::operator = ( const Image& _img )
	{
		if( m_buffer && m_autoDelete )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}
		m_width = _img.m_width;
		m_height = _img.m_height;
		m_depth = _img.m_depth;
		m_format = _img.m_format;
		m_size = _img.m_size;
		m_flags = _img.m_flags;
		m_pixelSize = _img.m_pixelSize;
		m_numMipmaps = _img.m_numMipmaps;
		m_autoDelete = _img.m_autoDelete;
		//Only create/copy when previous data was not dynamic data
		if( m_autoDelete )
		{
			m_buffer = new unsigned char[ m_size ];
			memcpy( m_buffer, _img.m_buffer, m_size );
		}
		else
		{
			m_buffer = _img.m_buffer;
		}

		return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	Image& Image::loadDynamicImage( unsigned char* _data, std::size_t _width, std::size_t _height, 
		std::size_t _depth,	PixelFormat _format, bool _autoDelete, std::size_t _numFaces, std::size_t _numMipMaps )
	{
		if( m_buffer && m_autoDelete )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}
		// Set image metadata
		m_width = _width;
		m_height = _height;
		m_depth = _depth;
		m_format = _format;
		m_pixelSize = static_cast<unsigned char>( PixelUtil::getNumElemBytes( m_format ) );
		m_numMipmaps = _numMipMaps;
		m_flags = 0;
		// Set flags
		if( PixelUtil::isCompressed( _format ) )
			m_flags |= IF_COMPRESSED;
		if( m_depth != 1 )
			m_flags |= IF_3D_TEXTURE;
		if( _numFaces == 6 )
			m_flags |= IF_CUBEMAP;
		if( _numFaces != 6 && _numFaces != 1 )
		{
			assert( 0 && "Image::loadDynamicImage -> Number of faces currently must be 6 or 1." );
		}

		m_size = calculateSize( _numMipMaps, _numFaces, _width, _height, _depth, _format );
		m_buffer = _data;
		m_autoDelete = _autoDelete;

		return *this;

	}
	//////////////////////////////////////////////////////////////////////////
	/*void Image::save( const String& _filename )
	{
		assert( m_buffer && "Image::save -> No image data loaded" );

		std::string strExt;
		std::size_t pos = _filename.find_last_of(".");
		assert( ( pos != std::string::npos ) && String(
			"Image::save -> Unable to save image file '" + _filename + "' - invalid extension.").c_str() );

		while( pos != _filename.length() - 1 )
			strExt += _filename[++pos];

		Codec * pCodec = Codec::getCodec( strExt );
		assert( pCodec && String(
			"Image::save -> Unable to save image file '" + _filename + "' - invalid extension.").c_str()
			);

		ImageCodec::ImageData* imgData = new ImageCodec::ImageData();
		imgData->format = m_format;
		imgData->height = m_height;
		imgData->width = m_width;
		imgData->depth = m_depth;
		// Wrap in CodecDataPtr, this will delete
		Codec::CodecData* codeDataPtr( imgData );
		// Wrap memory, be sure not to delete when stream destroyed
		//DataStreamInterface* wrapper = Holder<FileEngine>::hostage()
		//									->createMemoryFile( m_buffer, m_size, false );
		OutStreamInterface* outFile = Holder<FileEngine>::hostage()->openOutStream( _filename, true );

		//pCodec->codeToFile( wrapper, _filename, codeDataPtr );
		pCodec->code( outFile, m_buffer, codeDataPtr );

		Holder<FileEngine>::hostage()->closeOutStream( outFile );
	}*/
	//////////////////////////////////////////////////////////////////////////
	unsigned char* Image::getData()
	{
		return m_buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	const unsigned char* Image::getData() const
	{
		assert( m_buffer );
		return m_buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getNumMipmaps() const
	{
		return m_numMipmaps;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::hasFlag( const ImageFlags _imgFlag ) const
	{
		if( m_flags & _imgFlag )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getDepth() const
	{
		return m_depth;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getNumFaces() const
	{
		if( hasFlag( IF_CUBEMAP ) )
		{
			return 6;
		}
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Image::getRowSpan() const
	{
		return m_width * m_pixelSize;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat Image::getFormat() const
	{
		return m_format;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char Image::getBPP() const
	{
		return m_pixelSize * 8;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::getHasAlpha() const
	{
		return PixelUtil::getFlags( m_format ) & PFF_HASALPHA;
	}
	//////////////////////////////////////////////////////////////////////////
	void Image::applyGamma( unsigned char* _buffer, float _gamma, std::size_t _size, unsigned char _bpp )
	{
		if( _gamma == 1.0f )
			return;

		//NB only 24/32-bit supported
		if( _bpp != 24 && _bpp != 32 ) return;

		unsigned int stride = _bpp >> 3;

		for( std::size_t i = 0, j = _size / stride; i < j; i++, _buffer += stride )
		{
			float r, g, b;

			r = (float)_buffer[0];
			g = (float)_buffer[1];
			b = (float)_buffer[2];

			r = r * _gamma;
			g = g * _gamma;
			b = b * _gamma;

			float scale = 1.0f, tmp;

			if( r > 255.0f && ( tmp = ( 255.0f / r ) ) < scale )
			{
				scale = tmp;
			}
			if( g > 255.0f && ( tmp = ( 255.0f / g ) ) < scale )
			{
				scale = tmp;
			}
			if( b > 255.0f && ( tmp = ( 255.0f / b ) ) < scale )
			{
				scale = tmp;
			}

			r *= scale; g *= scale; b *= scale;

			_buffer[0] = (unsigned char)r;
			_buffer[1] = (unsigned char)g;
			_buffer[2] = (unsigned char)b;
		}
	}
	//////////////////////////////////////////////////////////////////////////    
	PixelBox Image::getPixelBox( std::size_t _face, std::size_t _mipmap ) const
	{
		// Image data is arranged as:
		// face 0, top level (mip 0)
		// face 0, mip 1
		// face 0, mip 2
		// face 1, top level (mip 0)
		// face 1, mip 1
		// face 1, mip 2
		// etc
		assert( ( _mipmap <= getNumMipmaps() ) && "Image::getPixelBox -> Mipmap index out of range" );
		assert( ( _face < getNumFaces() ) && "Image::getPixelBox -> Face index out of range" );
		// Calculate mipmap offset and size
		unsigned char* offset = const_cast<unsigned char*>( getData() );
		// Base offset is number of full faces
		std::size_t width = getWidth(), height = getHeight(), depth = getDepth();
		std::size_t numMips = getNumMipmaps();

		// Figure out the offsets 
		std::size_t fullFaceSize = 0;
		std::size_t finalFaceSize = 0;
		std::size_t finalWidth, finalHeight, finalDepth;
		for( std::size_t mip = 0; mip <= numMips; ++mip )
		{
			if ( mip == _mipmap )
			{
				finalFaceSize = fullFaceSize;
				finalWidth = width;
				finalHeight = height;
				finalDepth = depth;
			}
			fullFaceSize += PixelUtil::getMemorySize( width, height, depth, getFormat() );

			/// Half size in each dimension
			if( width != 1 ) width /= 2;
			if( height != 1 ) height /= 2;
			if( depth != 1 ) depth /= 2;
		}
		// Advance pointer by number of full faces, plus mip offset into
		offset += _face * fullFaceSize;
		offset += finalFaceSize;
		// Return subface as pixelbox
		PixelBox src( finalWidth, finalHeight, finalDepth, getFormat(), offset );
		return src;
	}
	//////////////////////////////////////////////////////////////////////////    
	std::size_t Image::calculateSize( std::size_t _mipmaps, std::size_t _faces, std::size_t _width, std::size_t _height, std::size_t _depth, 
		PixelFormat _format )
	{
		std::size_t size = 0;
		for( std::size_t mip = 0; mip <= _mipmaps; ++mip )
		{
			size += PixelUtil::getMemorySize( _width, _height, _depth, _format ) * _faces; 
			if( _width != 1 ) _width /= 2;
			if( _height != 1 ) _height /= 2;
			if( _depth != 1 ) _depth /= 2;
		}
		return size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
