
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
	Image& Image::flipAroundY()
	{
		assert( m_buffer &&
				"Image::flipAroundY -> Can not flip an unitialized texture" );

		m_numMipmaps = 0; // Image operations lose precomputed mipmaps

		unsigned char	*pTempBuffer1 = NULL;
		unsigned short	*pTempBuffer2 = NULL;
		unsigned char	*pTempBuffer3 = NULL;
		unsigned int	*pTempBuffer4 = NULL;

		unsigned char	*src1 = m_buffer, *dst1 = NULL;
		unsigned short	*src2 = (unsigned short *)m_buffer, *dst2 = NULL;
		unsigned char	*src3 = m_buffer, *dst3 = NULL;
		unsigned int	*src4 = (unsigned int *)m_buffer, *dst4 = NULL;

		unsigned short y;
		switch (m_pixelSize)
		{
		case 1:
			pTempBuffer1 = new unsigned char[m_width * m_height];
			for (y = 0; y < m_height; y++)
			{
				dst1 = (pTempBuffer1 + ((y * m_width) + m_width - 1));
				for (unsigned short x = 0; x < m_width; x++)
					memcpy(dst1--, src1++, sizeof(unsigned char));
			}

			memcpy(m_buffer, pTempBuffer1, m_width * m_height * sizeof(unsigned char));
			delete [] pTempBuffer1;
			break;

		case 2:
			pTempBuffer2 = new unsigned short[m_width * m_height];
			for (y = 0; y < m_height; y++)
			{
				dst2 = (pTempBuffer2 + ((y * m_width) + m_width - 1));
				for (unsigned short x = 0; x < m_width; x++)
					memcpy(dst2--, src2++, sizeof(unsigned short));
			}

			memcpy(m_buffer, pTempBuffer2, m_width * m_height * sizeof(unsigned short));
			delete [] pTempBuffer2;
			break;

		case 3:
			pTempBuffer3 = new unsigned char[m_width * m_height * 3];
			for (y = 0; y < m_height; y++)
			{
				std::size_t offset = ((y * m_width) + (m_width - 1)) * 3;
				dst3 = pTempBuffer3;
				dst3 += offset;
				for (std::size_t x = 0; x < m_width; x++)
				{
					memcpy(dst3, src3, sizeof(unsigned char) * 3);
					dst3 -= 3; src3 += 3;
				}
			}

			memcpy(m_buffer, pTempBuffer3, m_width * m_height * sizeof(unsigned char) * 3);
			delete [] pTempBuffer3;
			break;

		case 4:
			pTempBuffer4 = new unsigned int[m_width * m_height];
			for (y = 0; y < m_height; y++)
			{
				dst4 = (pTempBuffer4 + ((y * m_width) + m_width - 1));
				for (unsigned short x = 0; x < m_width; x++)
					memcpy(dst4--, src4++, sizeof(unsigned int));
			}

			memcpy(m_buffer, pTempBuffer4, m_width * m_height * sizeof(unsigned int));
			delete [] pTempBuffer4;
			break;

		default:
			assert( 0 && "Image::flipAroundY -> Unknown pixel depth" );
			break;
		}

		return *this;

	}
	//////////////////////////////////////////////////////////////////////////
	Image & Image::flipAroundX()
	{
		assert( m_buffer &&
				"Image::flipAroundX -> Can not flip an unitialized texture" );

		m_numMipmaps = 0; // Image operations lose precomputed mipmaps

		std::size_t rowSpan = m_width * m_pixelSize;

		unsigned char *pTempBuffer = new unsigned char[ rowSpan * m_height ];
		unsigned char *ptr1 = m_buffer, *ptr2 = pTempBuffer + ( ( m_height - 1 ) * rowSpan );

		for( unsigned short i = 0; i < m_height; i++ )
		{
			memcpy( ptr2, ptr1, rowSpan );
			ptr1 += rowSpan; ptr2 -= rowSpan;
		}

		memcpy( m_buffer, pTempBuffer, rowSpan * m_height );

		delete [] pTempBuffer;

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
	Image& Image::loadRawData(	DataStreamInterface* _stream, std::size_t _width, std::size_t _height, std::size_t _depth,
		PixelFormat _format, std::size_t _numFaces, std::size_t _numMipMaps )
	{
		std::size_t size = calculateSize( _numMipMaps, _numFaces, _width, _height, _depth, _format );
		assert( ( size == _stream->size() ) && 
				"Image::loadRawData -> Stream size does not match calculated image size" );

		unsigned char *buffer = new unsigned char[ size ];
		_stream->read( buffer, size );

		return loadDynamicImage( buffer, _width, _height, _depth, _format, true, _numFaces, _numMipMaps );

	}
	//////////////////////////////////////////////////////////////////////////
	Image& Image::load( const std::string& _strFileName )
	{
		if( m_buffer && m_autoDelete )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}

		String strExt;

		std::size_t pos = _strFileName.find_last_of(".");
		assert( pos != std::string::npos && String( "Image::load -> Unable to load image file '" 
													+ _strFileName + "' - invalid extension.").c_str() );

		while( pos != _strFileName.length() - 1 )
		{
			strExt += _strFileName[++pos];
		}

		Codec* codec = Codec::getCodec( strExt );

		if( !codec )
		{
			MENGE_LOG( "Warning: Image codec for extension %s was not found", strExt.c_str() );
		}

		DataStreamInterface* codeStream = Holder<FileEngine>::hostage()->openFile( _strFileName );

		if( !codeStream )
		{
			return *this;
		}

		ImageCodec::ImageData data;

		//ImageCodecPNG pngCodec;
		bool res = codec->getDataInfo( codeStream, static_cast<Codec::CodecData*>( &data ) );
		if( res == false )
		{
			MENGE_LOG( "Warning: Error while decoding image '%s'. Image not loaded", _strFileName.c_str() );
			Holder<FileEngine>::hostage()->closeStream( codeStream );
			return *this;
		}

		codeStream->seek( 0 );

		m_buffer = new unsigned char[data.size];

		res = codec->decode( codeStream, m_buffer, data.flags );
		if( res == false )
		{
			MENGE_LOG( "Warning: Error while decoding image '%s'. Image not loaded", _strFileName.c_str() );
			Holder<FileEngine>::hostage()->closeStream( codeStream );
			delete[] m_buffer;
			m_buffer = 0;
			return *this;
		}

		m_width = data.width;
		m_height = data.height;
		m_depth = data.depth;
		m_size = data.size;
		m_format = data.format;
		m_numMipmaps = data.num_mipmaps;
		m_pixelSize = static_cast<unsigned char>( PixelUtil::getNumElemBytes( m_format ) );
		m_flags = data.flags;


		//Codec::DecodeResult res = codec->decode( encoded );
	
		Holder<FileEngine>::hostage()->closeStream( codeStream );

		/*if( res.second == 0 )
		{
			MENGE_LOG( "Warning: Error while decoding image '%s'. Image not loaded", _strFileName.c_str() );
			return *this;
		}

		ImageCodec::ImageData* data = 
			static_cast<ImageCodec::ImageData*>( res.second );

		// Get the format and compute the pixel size
		m_width = data->width;
		m_height = data->height;
		m_depth = data->depth;
		m_size = data->size;
		m_format = data->format;
		m_numMipmaps = data->num_mipmaps;
		m_pixelSize = static_cast<unsigned char>( PixelUtil::getNumElemBytes( m_format ) );
		m_flags = data->flags;

		// re-use the decoded buffer
		m_buffer = static_cast<unsigned char*>( res.first->getBuffer() );
		// ensure we don't delete when stream is closed
		res.first->setFreeOnClose( false );

		Holder<FileEngine>::hostage()->closeStream( res.first );

		delete res.second;*/

		return *this;

	}
	//////////////////////////////////////////////////////////////////////////
	void Image::save( const std::string& _filename )
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
	}
	//////////////////////////////////////////////////////////////////////////
	Image& Image::load( DataStreamInterface* _stream, const std::string& _type )
	{
	/*	if( m_buffer && m_autoDelete )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}

		std::string strType = _type;

		Codec * codec = Codec::getCodec( strType );
		assert( codec && 
			"Image::load -> Unable to load image - invalid extension." );

		Codec::DecodeResult res = codec->decode( _stream );

		ImageCodec::ImageData* pData = 	static_cast<ImageCodec::ImageData*>( res.second );

		m_width = pData->width;
		m_height = pData->height;
		m_depth = pData->depth;
		m_size = pData->size;
		m_numMipmaps = pData->num_mipmaps;
		m_flags = pData->flags;

		// Get the format and compute the pixel size
		m_format = pData->format;
		m_pixelSize = static_cast<unsigned char>(PixelUtil::getNumElemBytes( m_format ));
		// Just use internal buffer of returned memory stream
		m_buffer = static_cast<unsigned char*>( res.first->getBuffer() );
		// Make sure stream does not delete
		res.first->setFreeOnClose( true );
*/
		return *this;
	}
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
	/*void Image::resize( unsigned short _width, unsigned short _height, Filter _filter )
	{
		// resizing dynamic images is not supported
		assert( m_autoDelete );
		assert( m_depth == 1 );

		// reassign buffer to temp image, make sure auto-delete is true
		Image temp;
		temp.loadDynamicImage( m_buffer, m_width, m_height, 1, m_format, true );
		// do not delete[] m_buffer!  temp will destroy it

		// set new dimensions, allocate new buffer
		m_width = _width;
		m_height = _height;
		m_size = PixelUtil::getMemorySize( m_width, m_height, 1, m_format );
		m_buffer = new unsigned char[m_size];
		m_numMipmaps = 0; // Loses precomputed mipmaps

		// scale the image from temp into our resized buffer
		Image::scale( temp.getPixelBox(), getPixelBox(), _filter );
	}*/
	//////////////////////////////////////////////////////////////////////////
	/*void Image::scale( const PixelBox& _src, const PixelBox& _scaled, Filter _filter ) 
	{
		assert( PixelUtil::isAccessible( _src.format ) );
		assert( PixelUtil::isAccessible( _scaled.format ) );
		DataStreamInterface* buf = NULL;
		PixelBox temp;
		switch( _filter ) 
		{
		default:
		case FILTER_NEAREST:
			if( _src.format == _scaled.format ) 
			{
				// No intermediate buffer needed
				temp = _scaled;
			}
			else
			{
				// Allocate temporary buffer of destination size in source format 
				temp = PixelBox( _scaled.getWidth(), _scaled.getHeight(), _scaled.getDepth(), _src.format );
				buf = new MemoryDataStream( temp.getConsecutiveSize() ) );
				temp.data = buf->getPtr();
			}
			// super-optimized: no conversion
			switch (PixelUtil::getNumElemBytes( _src.format )) 
			{
			case 1: NearestResampler<1>::scale( _src, temp ); break;
			case 2: NearestResampler<2>::scale( _src, temp ); break;
			case 3: NearestResampler<3>::scale( _src, temp ); break;
			case 4: NearestResampler<4>::scale( _src, temp ); break;
			case 6: NearestResampler<6>::scale( _src, temp ); break;
			case 8: NearestResampler<8>::scale( _src, temp ); break;
			case 12: NearestResampler<12>::scale( _src, temp ); break;
			case 16: NearestResampler<16>::scale( _src, temp ); break;
			default:
				// never reached
				assert( false );
			}
			if( temp.data != _scaled.data )
			{
				// Blit temp buffer
				PixelUtil::bulkPixelConversion( temp, _scaled );
			}
			break;

		case FILTER_LINEAR:
		case FILTER_BILINEAR:
			switch ( _src.format ) 
			{
			case PF_L8: case PF_A8: case PF_BYTE_LA:
			case PF_R8G8B8: case PF_B8G8R8:
			case PF_R8G8B8A8: case PF_B8G8R8A8:
			case PF_A8B8G8R8: case PF_A8R8G8B8:
			case PF_X8B8G8R8: case PF_X8R8G8B8:
				if( _src.format == _scaled.format ) 
				{
					// No intermediate buffer needed
					temp = _scaled;
				}
				else
				{
					// Allocate temp buffer of destination size in source format 
					temp = PixelBox( _scaled.getWidth(), _scaled.getHeight(), _scaled.getDepth(), _src.format );
					buf.bind( new MemoryDataStream( temp.getConsecutiveSize() ) );
					temp.data = buf->getPtr();
				}
				// super-optimized: byte-oriented math, no conversion
				switch ( PixelUtil::getNumElemBytes( _src.format ) ) 
				{
				case 1: LinearResampler_Byte<1>::scale( _src, temp ); break;
				case 2: LinearResampler_Byte<2>::scale( _src, temp ); break;
				case 3: LinearResampler_Byte<3>::scale( _src, temp ); break;
				case 4: LinearResampler_Byte<4>::scale( _src, temp ); break;
				default:
					// never reached
					assert( false );
				}
				if( temp.data != _scaled.data )
				{
					// Blit temp buffer
					PixelUtil::bulkPixelConversion( temp, _scaled );
				}
				break;
			case PF_FLOAT32_RGB:
			case PF_FLOAT32_RGBA:
				if (_scaled.format == PF_FLOAT32_RGB || _scaled.format == PF_FLOAT32_RGBA)
				{
					// float32 to float32, avoid unpack/repack overhead
					LinearResampler_Float32::scale( _src, _scaled );
					break;
				}
				// else, fall through
			default:
				// non-optimized: floating-point math, performs conversion but always works
				LinearResampler::scale( _src, _scaled );
			}
			break;
		}
	}*/
	//////////////////////////////////////////////////////////////////////////    
	/*ColourValue Image::getColourAt( int _x, int _y, int _z ) 
	{
		ColourValue rval;
		PixelUtil::unpackColour( &rval, m_format, &m_buffer[m_pixelSize * ( _z * m_width * m_height + m_width * _y + _x )] );
		return rval;
	}*/
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
