///*
// *  OGLRenderTextureCopy.cpp
// *  Mac
// *
// *  Created by Berserk on 13.11.08.
// *  Copyright 2008 __Menge__. All rights reserved.
// *
// */
//
//#include "OGLRenderTextureCopy.h"
//#include <cassert>
//
//OGLRenderTextureCopy::OGLRenderTextureCopy()
//: OGLTexture()
//{
//}
//
//OGLRenderTextureCopy::~OGLRenderTextureCopy()
//{
//	if( m_texture != 0 )
//	{
//		glDeleteTextures( 1 ,&m_texture );
//		m_texture = 0;
//	}
//}
//
//bool OGLRenderTextureCopy::create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height )
//{
//	std::size_t memSize = _data_width * _data_height * 3;
//	Menge::TextureDesc _desc = {_name,0,0,memSize,_data_width,_data_height,Menge::PF_R8G8B8};
//	OGLTexture::load( _width, _height, _desc );
//	/*m_format = Menge::PF_R8G8B8;
//	m_name = _name;
//	m_width = _data_width;
//	m_height = _data_height;
//	m_image_width = _width;
//	m_image_height = _height;
//	m_uvMask[0] = 1.0f;//static_cast<float>( m_image_width ) / m_width;
//	m_uvMask[1] = 1.0f;//static_cast<float>( m_image_height ) / m_height;
//	m_pitch = 0;//_desc.size / _desc.height;*/
//	return true;
//}
//
//bool OGLRenderTextureCopy::enable()
//{
//
//	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//	
//	return true;
//}
//
//void OGLRenderTextureCopy::flush()
//{
//	int pitch = 0;
//	unsigned char* imageData = lock( &pitch, false );
//	glReadBuffer( GL_BACK );
//	int x = 0;
//	int y = 0;//m_windowHeight;
//	std::size_t width =  1024;//m_windowWidth; 
//	std::size_t height = 768;//m_windowHeight; 
//
//	std::size_t iWidth = getWidth();
//	std::size_t iHeight = getHeight();
//
//	int bufDataPitch = width * 3;
//	unsigned char* bufferData = new unsigned char[bufDataPitch*height];
//	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
//	glReadPixels( x, y, width, height, GL_BGR, GL_UNSIGNED_BYTE, bufferData );	// why is our buffer flipped by y????
//	glPixelStorei( GL_PACK_ALIGNMENT, 4 );
//	if( iWidth == width && iHeight == height )
//	{
//		unsigned char* buf = bufferData + bufDataPitch * ( height - 1 );
//		for( std::size_t i = 0; i < height; i++ )
//		{
//			std::copy( buf, buf + bufDataPitch, imageData );
//			imageData += pitch;
//			buf -= bufDataPitch; // flip by y
//		}
//	}
//	else	// resample image
//	{
//
//		// srcdata stays at beginning of slice, pdst is a moving pointer
//		/*unsigned char* srcdata = bufferData;
//		unsigned char* pdst = imageData;// + ( iHeight - 1 ) * pitch;	// flip by y
//
//		// sx_48,sy_48 represent current position in source
//		// using 16/48-bit fixed precision, incremented by steps
//		Menge::uint64 stepx = ((Menge::uint64)width << 48) / iWidth;
//		Menge::uint64 stepy = ((Menge::uint64)height << 48) / iHeight;
//
//		// bottom 28 bits of temp are 16/12 bit fixed precision, used to
//		// adjust a source coordinate backwards by half a pixel so that the
//		// integer bits represent the first sample (eg, sx1) and the
//		// fractional bits are the blend weight of the second sample
//		unsigned int temp;
//
//		Menge::uint64 sy_48 = (stepy >> 1) - 1;
//		for (size_t y = 0; y < iHeight; y++, sy_48 += stepy) 
//		{
//			temp = sy_48 >> 36;
//			temp = (temp > 0x800)? temp - 0x800: 0;
//			unsigned int syf = temp & 0xFFF;
//			size_t sy1 = temp >> 12;
//			size_t sy2 = (std::min)( sy1 + 1, height - 1 );
//			size_t syoff1 = sy1 * width;//bufDataPitch;
//			size_t syoff2 = sy2 * width;//bufDataPitch;
//
//			Menge::uint64 sx_48 = (stepx >> 1) - 1;
//			for (size_t x = 0; x < iWidth; x++, sx_48+=stepx) 
//			{
//				temp = sx_48 >> 36;
//				temp = (temp > 0x800)? temp - 0x800 : 0;
//				unsigned int sxf = temp & 0xFFF;
//				size_t sx1 = temp >> 12;
//				size_t sx2 = (std::min)(sx1+1, width-1);
//
//				unsigned int sxfsyf = sxf*syf;
//				for (unsigned int k = 0; k < 4; k++) 
//				{
//					unsigned int accum =
//						srcdata[(sx1 + syoff1)*3+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
//						srcdata[(sx2 + syoff1)*3+k]*((sxf<<12)-sxfsyf) +
//						srcdata[(sx1 + syoff2)*3+k]*((syf<<12)-sxfsyf) +
//						srcdata[(sx2 + syoff2)*3+k]*sxfsyf;
//					// accum is computed using 8/24-bit fixed-point math
//					// (maximum is 0xFF000000; rounding will not cause overflow)
//					*pdst++ = (accum + 0x800000) >> 24;
//				}
//			}
//			//pdst -= pitch * 2;
//			//pdst += 4*dst.getRowSkip();
//		}*/
//
//	}
//	delete[] bufferData;
//	unlock();
//	
//	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//}
//
//unsigned char * OGLRenderTextureCopy::lock( int* _pitch, bool _readOnly )
//{
//	//assert(!"Not implemented");
//	//return NULL;
//	return OGLTexture::lock( _pitch, _readOnly );
//}
//
//void OGLRenderTextureCopy::unlock()
//{
//	OGLTexture::unlock();
//}
