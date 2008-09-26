#	include "ResourceImage.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"
#	include "Utils.h"

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
			->loadImage( _fileName, m_filter );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG_ERROR << "Warning: resource \"" << getName() << "\" can't load image file \"" << _fileName << "\"";
			imageFrame.image = 0;

			return imageFrame;
		}

		float width = (float)image->getWidth();
		float height = (float)image->getHeight();


		imageFrame.size = mt::vec2f( width, height );
		imageFrame.image = image;

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
			->createImage( _name, _size.x, _size.y );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG_ERROR << "Warning: resource \"" << getName() << "\" can't create image file \"" << _name << "\"";
			imageFrame.image = 0;

			return imageFrame;
		}

		imageFrame.size = _size;
		imageFrame.image = image;

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
}