#	pragma once

#	include "PixelFormat.h"
#	include "FileEngine.h"

namespace Menge
{
	enum ImageFlags
	{
		IF_COMPRESSED = 0x00000001,
		IF_CUBEMAP    = 0x00000002,
		IF_3D_TEXTURE = 0x00000004
	};
	//////////////////////////////////////////////////////////////////////////
	class Image
	{
	public:
		typedef Menge::Box Box;
	public:

		// Standard constructor.
		Image();

		// Copy-constructor - copies all the data from the target image.
		Image( const Image& _img );

		// Standard destructor.
		virtual ~Image();

		// Assignment operator - copies all the data from the target image.
		Image & operator = ( const Image & _img );

		//Flips (mirrors) the image around the Y-axis. 
		//@remarks
		//An example of an original and flipped image:
		//<pre>                
		//originalimg
		//00000000000
		//00000000000
		//00000000000
		//00000000000
		//00000000000
		//------------> flip axis
		//00000000000
		//00000000000
		//00000000000
		//00000000000
		//00000000000
		//originalimg
		//</pre>
		Image & flipAroundY();

		//Flips (mirrors) the image around the X-axis.
		//@remarks
		//An example of an original and flipped image:
		//<pre>
		//flip axis
		//|
		//originalimg|gmilanigiro
		//00000000000|00000000000
		//00000000000|00000000000
		//00000000000|00000000000
		//00000000000|00000000000
		//00000000000|00000000000
		//</pre>
		Image & flipAroundX();

		//Stores a pointer to raw data in memory. The pixel format has to be specified.
		//@remarks
		//This method loads an image into memory held in the object. The 
		//pixel format will be either greyscale or RGB with an optional
		//Alpha component.
		//The type can be determined by calling getFormat().             
		//@note
		//Whilst typically your image is likely to be a simple 2D image,
		//you can define complex images including cube maps, volume maps,
		//and images including custom mip levels. The layout of the 
		//internal memory should be:
		//<ul><li>face 0, mip 0 (top), width x height (x depth)</li>
		//<li>face 0, mip 1, width/2 x height/2 (x depth/2)</li>
		//<li>face 0, mip 2, width/4 x height/4 (x depth/4)</li>
		//<li>.. remaining mips for face 0 .. </li>
		//<li>face 1, mip 0 (top), width x height (x depth)</li
		//<li>.. and so on. </li>
		//</ul>
		//Of course, you will never have multiple faces (cube map) and
		//depth too.
		//@param
		//The data pointer
		//@param
		//Width of image
		//@param
		//Height of image
		//@param
		//Image Depth (in 3d images, numbers of layers, otherwhise 1)
		//@param
		//Pixel Format
		//@param
		//if memory associated with this buffer is to be destroyed
		//with the Image object.
		//@param
		//the number of faces the image data has inside (6 for cubemaps, 1 otherwise)
		//@param
		//the number of mipmaps the image data has inside
		//@note
		//The memory associated with this buffer is NOT destroyed with the
		//Image object, unless autoDelete is set to true.
		//@remarks 
		//The size of the buffer must be numFaces*PixelUtil::getMemorySize(width, height, depth, format)
		Image& loadDynamicImage( unsigned char* _data, std::size_t _width, std::size_t _height, 
			std::size_t _depth,	PixelFormat _format, bool _autoDelete = false, 
			std::size_t _numFaces = 1, std::size_t _numMipMaps = 0 );

		//Stores a pointer to raw data in memory. The pixel format has to be specified.
		//@remarks
		//This method loads an image into memory held in the object. The 
		//pixel format will be either greyscale or RGB with an optional
		//Alpha component.
		//The type can be determined by calling getFormat().             
		//@note
		//Whilst typically your image is likely to be a simple 2D image,
		//you can define complex images including cube maps
		//and images including custom mip levels. The layout of the 
		//internal memory should be:
		//<ul><li>face 0, mip 0 (top), width x height</li>
		//<li>face 0, mip 1, width/2 x height/2 </li>
		//<li>face 0, mip 2, width/4 x height/4 </li>
		//<li>.. remaining mips for face 0 .. </li>
		//<li>face 1, mip 0 (top), width x height (x depth)</li
		//<li>.. and so on. </li>
		//</ul>
		//Of course, you will never have multiple faces (cube map) and
		//depth too.
		//@param
		//The data pointer
		//@param
		//Width of image
		//@param
		//Height of image
		//@param
		//Pixel Format
		//@note
		//The memory associated with this buffer is NOT destroyed with the
		//Image object.
		//@remarks This function is deprecated; one should really use the
		//Image::loadDynamicImage(pData, width, height, depth, format, ...) to be compatible
		//with future Ogre versions.
		Image& loadDynamicImage( unsigned char* _data, std::size_t _width, std::size_t _height, PixelFormat _format )
		{
			return loadDynamicImage( _data, _width, _height, 1, _format );
		}

		//Loads raw data from a stream. See the function
		//loadDynamicImage for a description of the parameters.
		//@remarks 
		//The size of the buffer must be numFaces*PixelUtil::getMemorySize(width, height, depth, format)
		//@note
		//Whilst typically your image is likely to be a simple 2D image,
		//you can define complex images including cube maps
		//and images including custom mip levels. The layout of the 
		//internal memory should be:
		//<ul><li>face 0, mip 0 (top), width x height (x depth)</li>
		//<li>face 0, mip 1, width/2 x height/2 (x depth/2)</li>
		//<li>face 0, mip 2, width/4 x height/4 (x depth/4)</li>
		//<li>.. remaining mips for face 0 .. </li>
		//<li>face 1, mip 0 (top), width x height (x depth)</li
		//<li>.. and so on. </li>
		//</ul>
		//Of course, you will never have multiple faces (cube map) and
		//depth too.
		Image & loadRawData( DataStreamInterface* _stream, std::size_t _width, std::size_t _height, std::size_t _depth,
			PixelFormat _format, std::size_t _numFaces = 1, std::size_t _numMipMaps = 0 );
		
		//Loads raw data from a stream. The pixel format has to be specified. 
		//@remarks This function is deprecated; one should really use the
		//Image::loadRawData(stream, width, height, depth, format, ...) to be compatible
		//with future Ogre versions.
		//@note
		//Whilst typically your image is likely to be a simple 2D image,
		//you can define complex images including cube maps
		//and images including custom mip levels. The layout of the 
		//internal memory should be:
		//<ul><li>face 0, mip 0 (top), width x height</li>
		//<li>face 0, mip 1, width/2 x height/2 </li>
		//<li>face 0, mip 2, width/4 x height/4 </li>
		//<li>.. remaining mips for face 0 .. </li>
		//<li>face 1, mip 0 (top), width x height (x depth)</li
		//<li>.. and so on. </li>
		//</ul>
		//Of course, you will never have multiple faces (cube map) and
		//depth too.
		Image& loadRawData( DataStreamInterface* _stream, std::size_t _width, std::size_t _height, 
			PixelFormat _format )
		{
			return loadRawData( _stream, _width, _height, 1, _format );
		}

		//Loads an image file.
		//@remarks
		//This method loads an image into memory. Any format for which 
		//and associated ImageCodec is registered can be loaded. 
		//This can include complex formats like DDS with embedded custom 
		//mipmaps, cube faces and volume textures.
		//The type can be determined by calling getFormat().             
		//@param
		//strFileName Name of a file file to load.
		//@param
		//groupName Name of the resource group to search for the image
		//@note
		//The memory associated with this buffer is destroyed with the
		//Image object.
		Image & load( const std::string& _strFileName );

		//Loads an image file from a stream.
		//@remarks
		//This method works in the same way as the filename-based load 
		//method except it loads the image from a DataStream object. 
		//This DataStream is expected to contain the 
		//encoded data as it would be held in a file. 
		//Any format for which and associated ImageCodec is registered 
		//can be loaded. 
		//This can include complex formats like DDS with embedded custom 
		//mipmaps, cube faces and volume textures.
		//The type can be determined by calling getFormat().             
		//@param
		//stream The source data.
		//@param
		//type The type of the image. Used to decide what decompression
		//codec to use.
		//@see
		//Image::load( const String& strFileName )
		Image& load( DataStreamInterface* _stream, const std::string& _type );

		// Save the image as a file.
		void save( const std::string& _filename );

		//Returns a pointer to the internal image buffer.
		//@remarks
		//Be careful with this method. You will almost certainly
		//prefer to use getPixelBox, especially with complex images
		//which include many faces or custom mipmaps.
		unsigned char* getData();

		//Returns a const pointer to the internal image buffer.
		//@remarks
		//Be careful with this method. You will almost certainly
		//prefer to use getPixelBox, especially with complex images
		//which include many faces or custom mipmaps.
		const unsigned char* getData() const;       

		// Returns the size of the data buffer.
		std::size_t getSize() const;

		// Returns the number of mipmaps contained in the image.
		std::size_t getNumMipmaps() const;

		// Returns true if the image has the appropriate flag set.
		bool hasFlag( const ImageFlags _imgFlag ) const;

		// Gets the width of the image in pixels.
		std::size_t getWidth() const;

		// Gets the height of the image in pixels.
		std::size_t getHeight() const;

		// Gets the depth of the image.
		std::size_t getDepth() const;

		// Get the numer of faces of the image. This is usually 6 for a cubemap, and
		// 1 for a normal image.
		std::size_t getNumFaces() const;

		// Gets the physical width in bytes of each row of pixels.
		std::size_t getRowSpan() const;

		// Returns the image format.
		PixelFormat getFormat() const;

		// Returns the number of bits per pixel.
		unsigned char getBPP() const;

		// Returns true if the image has an alpha component.
		bool getHasAlpha() const;

		//Does gamma adjustment.
		//@note
		//Basic algo taken from Titan Engine, copyright (c) 2000 Ignacio 
		//Castano Iguado
		static void applyGamma( unsigned char* _buffer, float _gamma, std::size_t _size, unsigned char _bpp );
	
		//Get colour value from a certain location in the image. The z coordinate
		//is only valid for cubemaps and volume textures. This uses the first (largest)
		//mipmap.
		//ColourValue getColourAt( int _x, int _y, int _z );

		// Get a PixelBox encapsulating the image data of a mipmap
		PixelBox getPixelBox( std::size_t _face = 0, std::size_t _mipmap = 0 ) const;

		enum Filter
		{
			FILTER_NEAREST,
			FILTER_LINEAR,
			FILTER_BILINEAR,
			FILTER_BOX,
			FILTER_TRIANGLE,
			FILTER_BICUBIC
		};

		//Scale a 1D, 2D or 3D image volume. 
		//@param 	src			PixelBox containing the source pointer, dimensions and format
		//@param 	dst			PixelBox containing the destination pointer, dimensions and format
		//@param 	filter		Which filter to use
		//@remarks 	This function can do pixel format conversion in the process.
		//@note	dst and src can point to the same PixelBox object without any problem
		//static void scale( const PixelBox& _src, const PixelBox& _dst, Filter _filter = FILTER_BILINEAR );

		/** Resize a 2D image, applying the appropriate filter. */
		//void resize( unsigned short _width, unsigned short _height, Filter _filter = FILTER_BILINEAR );

		// Static function to calculate size in bytes from the number of mipmaps, faces and the dimensions
		static std::size_t calculateSize( std::size_t _mipmaps, std::size_t _faces, std::size_t _width, std::size_t _height, std::size_t _depth, PixelFormat _format );

	private:
		// The width of the image in pixels
		std::size_t m_width;
		// The height of the image in pixels
		std::size_t m_height;
		// The depth of the image
		std::size_t m_depth;
		// The size of the image buffer
		std::size_t m_size;
		// The number of mipmaps the image contains
		std::size_t m_numMipmaps;
		// Image specific flags.
		int m_flags;

		// The pixel format of the image
		PixelFormat m_format;

		// The number of bytes per pixel
		unsigned char m_pixelSize;
		unsigned char* m_buffer;

		// A bool to determine if we delete the buffer or the calling app does
		bool m_autoDelete;	
	};

}	// namespace Menge