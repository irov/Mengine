#	include "Image.h"

#	include "Interface/InputSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	extern ServiceProviderInterface * serviceProvider;
	//////////////////////////////////////////////////////////////////////////
	Image::Image()
		: m_memory(nullptr)
		, m_width(0)
		, m_height(0)
		, m_channels(0)
		, m_embed(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Image::~Image()
	{
		delete [] m_memory;

		if( m_embed != nullptr )
		{
			pybind::unwrap( m_embed );
			pybind::decref( m_embed );

			m_embed = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::load( const FilePath & _path )
	{
		InputStreamInterfacePtr stream = FILE_SERVICE(serviceProvider)
			->openInputFile( ConstString::none(), _path, false );

		if( stream == false )
		{
			return false;
		}

		const ConstString & codecType = CODEC_SERVICE(serviceProvider)
			->findCodecType( _path );

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( codecType );

		if( imageDecoder == nullptr )
		{
			return false;
		}

		if( imageDecoder->prepareData( stream ) == false )
		{
			return false;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		m_path = _path;

		m_width = dataInfo->width;
		m_height = dataInfo->height;
		m_channels = dataInfo->channels;

		m_memory = new uint8_t[m_width * m_height * m_channels];

		ImageCodecOptions options;
		options.channels = m_channels;
		options.pitch = m_width * m_channels;

		if( imageDecoder->setOptions( &options ) == false )
		{
			return false;
		}

		size_t texture_size = dataInfo->getSize();

		if( imageDecoder->decode( m_memory, texture_size ) == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::save( const FilePath & _path )
	{
		OutputStreamInterfacePtr stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), _path );

		if( stream == nullptr )
		{
			return false;
		}

		const ConstString & codecType = CODEC_SERVICE(serviceProvider)
			->findCodecType( _path );

		ImageEncoderInterfacePtr encoder = CODEC_SERVICE(serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( codecType );

		if( encoder == nullptr )
		{
			return false;
		}

		if( encoder->initialize( stream ) == false )
		{
			return false;
		}

		ImageCodecOptions encode_options;

		encode_options.pitch = m_width * m_channels;
		encode_options.channels = m_channels;

		encoder->setOptions( &encode_options );

		ImageCodecDataInfo di;

		di.mipmaps = 0;
		di.width = m_width;
		di.height = m_height;
		di.channels = m_channels;
		di.depth = 1;
		di.quality = 100;

		encoder->encode( m_memory, &di );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::create( uint32_t _width, uint32_t _height, uint32_t _channel )
	{
		m_width = _width;
		m_height = _height;
		m_channels = _channel;
		
		m_memory = new uint8_t[m_width * m_height * m_channels];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Image::fill( const ColourValue & _colour )
	{
		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t index = (i + (j * m_width)) * m_channels;

				if( m_channels == 4 )
				{
					m_memory[index + 0] = static_cast<uint8_t>(_colour.getR() * 255.f);
					m_memory[index + 1] = static_cast<uint8_t>(_colour.getG() * 255.f);
					m_memory[index + 2] = static_cast<uint8_t>(_colour.getB() * 255.f);
					m_memory[index + 3] = static_cast<uint8_t>(_colour.getA() * 255.f);
				}
				else if( m_channels == 3 )
				{
					m_memory[index + 0] = static_cast<uint8_t>(_colour.getR() * 255.f);
					m_memory[index + 1] = static_cast<uint8_t>(_colour.getG() * 255.f);
					m_memory[index + 2] = static_cast<uint8_t>(_colour.getB() * 255.f);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::paste( Image * _image, uint32_t _x, uint32_t _y )
	{
		uint32_t paste_width = _image->getWidth();
		uint32_t paste_height = _image->getHeight();
		uint32_t paste_channels = _image->getChannels();

		uint32_t offset_end_x = _x + _image->getWidth();
		uint32_t offset_end_y = _y + _image->getHeight();

		if( offset_end_x > m_width || offset_end_y > m_height )
		{
			return false;
		}
		
		uint8_t * paste_memory = _image->getMemory();

		for( uint32_t j = 0; j != paste_height; ++j )
		{
			for( uint32_t i = 0; i != paste_width; ++i )
			{
				uint32_t paste_index = (i + (j * paste_width)) * paste_channels;

				uint32_t to_index = ((i + _x) + ((j + _y) * m_width)) * m_channels;

				m_memory[to_index + 0] = paste_memory[paste_index + 0];
				m_memory[to_index + 1] = paste_memory[paste_index + 1];
				m_memory[to_index + 2] = paste_memory[paste_index + 2];

				if( paste_channels == 4 )
				{
					m_memory[to_index + 3] = paste_memory[paste_index + 3];
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	uint8_t * Image::getMemory() const
	{
		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Image::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Image::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Image::getChannels() const
	{
		return m_channels;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Image::getdata() const
	{
		PyObject * pixels = pybind::list_new( m_width * m_height );

		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t pixel_index = i + (j * m_width);
				uint32_t index = pixel_index * m_channels;

				PyObject * pixel = pybind::tuple_new( m_channels );

				for( uint32_t k = 0; k != m_channels; ++k )
				{
					PyObject * color = pybind::ptr( m_memory[index + k] );

					pybind::tuple_setitem( pixel, k, color );
				}

				pybind::list_setitem( pixels, pixel_index, pixel );
			}
		}

		return pixels;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::putdata( PyObject * _data )
	{
		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t pixel_index = i + (j * m_width);
				uint32_t index = pixel_index * m_channels;

				PyObject * pixel = pybind::list_getitem( _data, pixel_index );

				for( uint32_t k = 0; k != m_channels; ++k )
				{
					PyObject * color = pybind::tuple_getitem( pixel, k );

					m_memory[index + k] = pybind::extract<uint8_t>( color );
				}								
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Image * Image::rotate( float _angle )
	{
		Image * image = new Image();

		image->create( m_height, m_width, m_channels );

		uint8_t * rotate_memory = image->getMemory();

		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t index = (i + (j * m_width)) * m_channels;
				uint32_t rotate_index = ((m_height - j - 1) + (i * m_height)) * m_channels;				

				for( uint32_t k = 0; k != m_channels; ++k )
				{
					rotate_memory[rotate_index + k] = m_memory[index + k];
				}
			}
		}

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Image::getextrema() const
	{
		uint8_t min_color[4];
		uint8_t max_color[4];

		for( uint32_t k = 0; k != m_channels; ++k )
		{
			min_color[k] = 255;
			max_color[k] = 0;
		}

		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t index = (i + (j * m_width)) * m_channels;

				for( uint32_t k = 0; k != m_channels; ++k )
				{
					uint8_t color = m_memory[index + k];

					if( min_color[k] > color )
					{
						min_color[k] = color;
					}

					if( max_color[k] < color )
					{
						max_color[k] = color;
					}
				}
			}
		}

		PyObject * py_extrema = pybind::list_new( m_channels );

		for( uint32_t k = 0; k != m_channels; ++k )
		{
			PyObject * py_minmax = pybind::tuple_new( 2 );

			PyObject * py_min = pybind::ptr( min_color[k] );
			PyObject * py_max = pybind::ptr( max_color[k] );

			pybind::tuple_setitem( py_minmax, 0, py_min );
			pybind::tuple_setitem( py_minmax, 1, py_max );

			pybind::list_setitem( py_extrema, k, py_minmax );
		}

		return py_extrema;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Image::uselessalpha() const
	{
		if( m_channels != 4 )
		{
			return true;
		}

		uint8_t min_alpha = 255;

		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t index = (i + (j * m_width)) * 4;

				uint8_t color = m_memory[index + 3];

				if( min_alpha > color )
				{
					min_alpha = color;
				}
			}
		}

		if( min_alpha != 255 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Image::split() const
	{
		Image * imageRGB = new Image;
		imageRGB->create(m_width, m_height, 3);

		Image * imageAlpha = new Image;
		imageAlpha->create(m_width, m_height, 1);

		uint8_t * memory_rgb = imageRGB->getMemory();
		uint8_t * memory_alpha = imageAlpha->getMemory();

		for( uint32_t j = 0; j != m_height; ++j )
		{
			for( uint32_t i = 0; i != m_width; ++i )
			{
				uint32_t index = (i + (j * m_width)) * m_channels;
				uint32_t index_rgb = (i + (j * m_width)) * 3;
				uint32_t index_a = (i + (j * m_width)) * 1;

				memory_rgb[index_rgb + 0] = m_memory[index + 0];
				memory_rgb[index_rgb + 1] = m_memory[index + 1];
				memory_rgb[index_rgb + 2] = m_memory[index + 2];

				memory_alpha[index_a + 0] = m_memory[index + 3];
			}
		}

		PyObject * py_split = pybind::tuple_new(2);

		PyObject * py_rgb = pybind::ptr(imageRGB);
		PyObject * py_alpha = pybind::ptr(imageAlpha);

		pybind::tuple_setitem(py_split, 0, py_rgb);
		pybind::tuple_setitem(py_split, 1, py_alpha);

		return py_split;
	}
	//////////////////////////////////////////////////////////////////////////
	void Image::embedding( PyObject * _module )
	{
		pybind::class_<Image>("Image", true, _module)
			.def( "getWidth", &Image::getWidth )
			.def( "getHeight", &Image::getHeight )
			.def( "getChannels", &Image::getChannels )
			.def( "paste", &Image::paste )
			.def( "getdata", &Image::getdata )
			.def( "putdata", &Image::putdata )
			.def( "rotate", &Image::rotate )
			.def( "getextrema", &Image::getextrema )
			.def( "uselessalpha", &Image::uselessalpha )
			.def( "split", &Image::split)
			;		
	}
}