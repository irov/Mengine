#	include "ResourceImage.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "Logger/Logger.h"
#	include "Texture.h"

#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage()
		: m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::loadImageFrame_( const ConstString& _pakName, const ConstString& _fileName, const ConstString& _codec ) const
	{
		Texture* texture = RenderEngine::get()
			->loadTexture( _pakName, _fileName, _codec );

		ImageFrame imageFrame;

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't load image file '%s'"
				, getName().c_str()
				, _fileName.c_str() 
				);

			imageFrame.texture = 0;

			return imageFrame;
		}

		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();


		imageFrame.size = mt::vec2f( width, height );
		imageFrame.texture = texture;
		if( texture->getPixelFormat() == PF_R8G8B8 )
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
	ConstString ResourceImage::s_getImageCodec( const String & _filename )
	{
		String codecType;
		Utils::getFileExt( codecType, _filename );
		codecType += "Image";

		return ConstManager::get()
			->genString( codecType );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::releaseImageFrame_(const ImageFrame & _frame) const
	{
		Holder<RenderEngine>::get()
			->releaseTexture( _frame.texture );
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
	ResourceImage::ImageFrame ResourceImage::createImageFrame_( const ConstString& _name, const mt::vec2f& _size ) const
	{
		Texture* texture = RenderEngine::get()
			->createTexture( _name, ::floorf( _size.x + 0.5f ), ::floorf( _size.y + 0.5f ), Menge::PF_A8R8G8B8 );

		ImageFrame imageFrame;

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't create image file '%s'"
				, getName().c_str()
				, _name.c_str() 
				);

			imageFrame.texture = 0;

			return imageFrame;
		}

		// fill with white color
		int pitch = 0;
		unsigned char* tData = texture->lock( &pitch, false );
		std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		texture->unlock();

		imageFrame.size = _size;
		imageFrame.texture = texture;

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ImageFrame ResourceImage::createRenderTargetFrame_( const ConstString& _name, const mt::vec2f& _size ) const
	{
		Texture* texture = RenderEngine::get()
			->createRenderTargetTexture( _name, _size );

		ImageFrame imageFrame;

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't create image file '%s'"
				, getName().c_str()
				, _name.c_str() 
				);

			imageFrame.texture = 0;

			return imageFrame;
		}

		// fill with white color
		//int pitch = 0;
		//unsigned char* tData = image->lock( &pitch, false );
		//std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		//image->unlock();

		imageFrame.size = _size;
		imageFrame.texture = texture;

		return imageFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::getWrapX( std::size_t _frame ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::getWrapY( std::size_t _frame ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}
