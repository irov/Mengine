#	include "ResourceImage.h"

#	include "BinParser.h"

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
	bool ResourceImage::loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const ConstString& _fileName, const ConstString& _codec ) const
	{
		Texture* texture = RenderEngine::get()
			->loadTexture( _pakName, _fileName, _codec );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't load image file '%s'"
				, getName().c_str()
				, _fileName.c_str() 
				);

			return false;
		}

		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();

		_frame.size = mt::vec2f( width, height );

		float hwWidth = (float)texture->getHWWidth();
		float hwHeight = (float)texture->getHWHeight();

		_frame.uv_pow.x = width / hwWidth;
		_frame.uv_pow.y = height / hwHeight;
		
		_frame.texture = texture;
		if( texture->getPixelFormat() == PF_R8G8B8 )
		{
			_frame.isAlpha = false;
		}
		else
		{
			_frame.isAlpha = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString ResourceImage::s_getImageCodec( const ConstString & _filename )
	{
		String codecType;
		Utils::getFileExt( codecType, _filename );
		codecType += "Image";

		return ConstString(codecType);
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::releaseImageFrame_(const ImageFrame & _frame) const
	{
		RenderEngine::get()
			->releaseTexture( _frame.texture );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Filter_Value, m_filter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const
	{
		Texture* texture = RenderEngine::get()
			->createTexture( _name, ::floorf( _size.x + 0.5f ), ::floorf( _size.y + 0.5f ), Menge::PF_A8R8G8B8 );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't create image file '%s'"
				, getName().c_str()
				, _name.c_str() 
				);

			return false;
		}

		// fill with white color
		int pitch = 0;
		unsigned char* tData = texture->lock( &pitch, false );
		std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		texture->unlock();

		_frame.size = _size;
		_frame.texture = texture;

		float hwWidth = (float)texture->getHWWidth();
		float hwHeight = (float)texture->getHWHeight();

		_frame.uv_pow.x = _size.x / hwWidth;
		_frame.uv_pow.y = _size.y / hwHeight;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::createRenderTargetFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const
	{
		Texture* texture = RenderEngine::get()
			->createRenderTargetTexture( _name, _size );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't create image file '%s'"
				, getName().c_str()
				, _name.c_str() 
				);

			return false;
		}

		// fill with white color
		//int pitch = 0;
		//unsigned char* tData = image->lock( &pitch, false );
		//std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
		//image->unlock();

		_frame.size = _size;
		_frame.texture = texture;

		float hwWidth = (float)texture->getHWWidth();
		float hwHeight = (float)texture->getHWHeight();

		_frame.uv_pow.x = _size.x / hwWidth;
		_frame.uv_pow.y = _size.y / hwHeight;

		return true;
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
