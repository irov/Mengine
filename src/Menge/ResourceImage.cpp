#	include "ResourceImage.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage( const std::string & _name )
		: ResourceReference( _name )
		, m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::loadImageFrame( const std::string & _fileName )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->loadImage( _fileName, m_filter );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load image file '%s'\n"
				, m_name.c_str()
				, _fileName.c_str()
				);

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
	void ResourceImage::releaseImageFrame( const ImageFrame & _frame )
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
			XML_CASE_VALUE_NODE( "Filter", "Value", m_filter );
		}
	}
}