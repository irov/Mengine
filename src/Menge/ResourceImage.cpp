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
	ResourceImage::ImageFrame ResourceImage::loadImageFrame( const std::string & _fileName )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->loadImage( _fileName, m_filter );

		ImageFrame imageFrame;

		if( image == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load image file '%s'\n"
				, getName().c_str()
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
	void ResourceImage::releaseImageFrame( const std::string & _description )
	{
		Holder<RenderEngine>::hostage()
			->releaseImage( _description );
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
}