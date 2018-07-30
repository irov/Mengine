#	pragma once

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Kernel/ColourValue.h"

#	include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class Image
	{
	public:
		Image();
		~Image();

	public:
		bool load( const FilePath & _path );
		bool save( const FilePath & _path );
		
		bool create( uint32_t _width, uint32_t _height, uint32_t _channel );
		
		void fill( const ColourValue & _colour );

		bool paste( Image * _image, uint32_t _x, uint32_t _y );

		pybind::list getdata() const;
		bool putdata( const pybind::list & _data );

		Image * rotate( float _angle );

		pybind::list getextrema() const;
		bool uselessalpha() const;

		pybind::tuple split() const;

	public:
		uint8_t * getMemory() const;

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getChannels() const;

	public:
		static void embedding( pybind::kernel_interface * _kernel, PyObject * _module );

	protected:
		FilePath m_path;

		MemoryBufferInterfacePtr m_memory;

		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_channels;

		PyObject * m_embed;
	};
}