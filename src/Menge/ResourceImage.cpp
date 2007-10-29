#	include "ResourceImage.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage( const std::string & _name )
		: ResourceReference( _name )
		, m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::loadImageFrame( const std::string & _fileName, std::vector<char> & _buff )
	{
		RenderImageInterface * image = 
			Holder<RenderEngine>::hostage()
			->loadImage( _fileName, _buff, m_filter );

		float width = (float)image->getWidth();
		float height = (float)image->getHeight();

		ImageFrame imageFrame;
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
	void ResourceImage::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Filter", "Value", m_filter );
		}
	}
}