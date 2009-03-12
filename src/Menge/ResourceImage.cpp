#	include "ResourceImage.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::loadImageFrame( const String& _fileName )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->loadImage( _fileName );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource \"%s\" can't load image file \"%s\""
			, getName().c_str()
			, _fileName.c_str() );
			imageFrame.image = 0;

			return imageFrame;
		}

		float width = (float)image->getWidth();
		float height = (float)image->getHeight();


		imageFrame.size = mt::vec2f( width, height );
		imageFrame.image = image;
		if( image->getPixelFormat() == PF_R8G8B8 )
		{
			imageFrame.isAlpha = false;
		}
		else
		{
			imageFrame.isAlpha = true;
		}

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::releaseImageFrame(const ImageFrame & _frame)
	{
		Holder<RenderEngine>::hostage()
			->releaseImage( _frame.image );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Filter", "Value", m_filter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::createImageFrame( const String& _name, const mt::vec2f& _size )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->createImage( _name, _size.x, _size.y, Menge::PF_A8R8G8B8 );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource \"%s\" can't create image file \"%s\""
			, getName().c_str()
			, _name.c_str() );
			imageFrame.image = 0;

			return imageFrame;
		}

		// fill with white color
		int pitch = 0;
		unsigned char* tData = image->lock( &pitch, false );
		std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		image->unlock();

		imageFrame.size = _size;
		imageFrame.image = image;

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::createRenderTargetFrame( const String& _name, const mt::vec2f& _size )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->createRenderTargetImage( _name, _size );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource \"%s\" can't create image file \"%s\""
				, getName().c_str()
				, _name.c_str() );
			imageFrame.image = 0;

			return imageFrame;
		}

		// fill with white color
		//int pitch = 0;
		//unsigned char* tData = image->lock( &pitch, false );
		//std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		//image->unlock();

		imageFrame.size = _size;
		imageFrame.image = image;

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
}
