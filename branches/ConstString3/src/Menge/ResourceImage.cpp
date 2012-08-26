#	include "ResourceImage.h"

#	include "RenderEngine.h"
#	include "LogEngine.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"

#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage()
		: m_filter(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::prepareImageFrame_( ImageFrame & _frame, RenderTextureInterface* texture ) const
	{
		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();

		_frame.size.x = width;
		_frame.size.y = height;

		const Rect & rect = texture->getRect();
		
        size_t hwWidth = texture->getHWWidth();
        size_t hwHeight = texture->getHWHeight();

        _frame.uv_scale.x = float(rect.left) / float(hwWidth);
        _frame.uv_scale.y = float(rect.top) / float(hwHeight);
        _frame.uv_scale.z = float(rect.right) / float(hwWidth);
        _frame.uv_scale.w = float(rect.bottom) / float(hwHeight);

        //_frame.uv_scale = mt::vec4f(0.f, 0.f, 1.f, 1.f);

		_frame.texture = texture;

		PixelFormat pixelFrormat = texture->getPixelFormat();

		if( pixelFrormat == PF_R8G8B8 )
		{
			_frame.isAlpha = false;
		}
		else
		{
			_frame.isAlpha = true;
		}

        _frame.uv_image = _frame.uv;

        float ku = _frame.uv.z - _frame.uv.x;
        float kv = _frame.uv.w - _frame.uv.y;
        _frame.size.x *= ku;
        _frame.size.y *= kv;

        _frame.uv.x = _frame.uv_scale.x + (_frame.uv_scale.z - _frame.uv_scale.x) * _frame.uv.x;
        _frame.uv.y = _frame.uv_scale.y + (_frame.uv_scale.w - _frame.uv_scale.y) * _frame.uv.y;
        _frame.uv.z = _frame.uv_scale.x + (_frame.uv_scale.z - _frame.uv_scale.x) * _frame.uv.z;
        _frame.uv.w = _frame.uv_scale.y + (_frame.uv_scale.w - _frame.uv_scale.y) * _frame.uv.w;

        if( _frame.maxSize.x < 0.f || _frame.maxSize.y < 0.f )
        {
            _frame.maxSize = _frame.size;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const WString& _fileName, const ConstString& _codec ) const
	{
		RenderTextureInterface* texture = RenderEngine::get()
			->loadTexture( _pakName, _fileName, _codec );

		//RenderTextureInterface* texture = RenderEngine::get()
		//	->loadMegatexture( _pakName, _fileName, _codec );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImage::loadImageFrame_: '%s' can't load image file '%S'"
				, this->getName().c_str()
				, _fileName.c_str()
				);

			return false;
		}

		if( this->prepareImageFrame_( _frame, texture ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImage::s_getImageCodec( const WString & _filename )
	{
		WString codecExt;
		Utils::getFileExt( _filename, codecExt );

		const ConstString & codecType = CodecEngine::get()
			->findCodecType( codecExt );

		return codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::releaseImageFrame_(const ImageFrame & _frame) const
	{
		RenderEngine::get()
			->releaseTexture( _frame.texture );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::loader( const Metabuf::Metadata * _parser )
	{
		//ResourceReference::loader( _parser );

		//BIN_SWITCH_ID( _parser )
		//{
		//	BIN_CASE_ATTRIBUTE( Protocol::Filter_Value, m_filter );
		//}
	}
	////////////////////////////////////////////////////////////////////////////
	//bool ResourceImage::createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const
	//{
	//	//::floorf( _size.x + 0.5f ), ::floorf( _size.y + 0.5f )
	//	RenderTextureInterface* texture = RenderEngine::get()
	//		->createTexture( _name, _size.x, _size.y, Menge::PF_A8R8G8B8 );

	//	if( texture == 0 )
	//	{
	//		MENGE_LOG_ERROR( "Warning: resource '%s' can't create image file '%s'"
	//			, getName().c_str()
	//			, _name.c_str() 
	//			);

	//		return false;
	//	}

	//	// fill with white color
	//	int pitch = 0;
	//	unsigned char* tData = texture->lock( &pitch, false );
	//	std::fill( tData, tData + pitch * (int)_size.y, 0xFF );
	//	texture->unlock();

	//	_frame.size = _size;
	//	_frame.texture = texture;

	//	return true;
	//}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::getWrapX() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::getWrapY() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderInterface * ResourceImage::createDecoder_(  FileInputStreamInterface * _file, const ConstString& _codec ) const
	{
		ImageDecoderInterface * imageDecoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codec, _file );

		if( imageDecoder == 0 )
		{
			return NULL;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

		if( dataInfo->format == PF_UNKNOWN )
		{
			imageDecoder->destroy();
			return NULL;
		}

		return imageDecoder;
	}
	////////////////////////////////////////////////////////////////////////////
	//size_t ResourceImage::getCount() const
	//{
	//	return 1;
	//}
	//////////////////////////////////////////////////////////////////////////
}
