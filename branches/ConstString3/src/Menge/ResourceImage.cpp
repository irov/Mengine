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
		
        size_t hwWidth = (float)texture->getHWWidth();
        size_t hwHeight = (float)texture->getHWHeight();

        _frame.maxSize.x = hwWidth;
        _frame.maxSize.y = hwHeight;

        _frame.uv_scale.x = float(rect.left) / hwWidth;
        _frame.uv_scale.y = float(rect.top) / hwHeight;
        _frame.uv_scale.z = float(rect.right) / hwWidth;
        _frame.uv_scale.w = float(rect.bottom) / hwHeight;

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

        float ku = _frame.uv.z - _frame.uv.x;
        float kv = _frame.uv.w - _frame.uv.y;
        _frame.size.x *= ku;
        _frame.size.y *= kv;

        _frame.uv_image.x = _frame.uv_scale.x + (_frame.uv_scale.z - _frame.uv_scale.x) * _frame.uv.x;
        _frame.uv_image.y = _frame.uv_scale.y + (_frame.uv_scale.w - _frame.uv_scale.y) * _frame.uv.y;
        _frame.uv_image.z = _frame.uv_scale.x + (_frame.uv_scale.z - _frame.uv_scale.x) * _frame.uv.z;
        _frame.uv_image.w = _frame.uv_scale.y + (_frame.uv_scale.w - _frame.uv_scale.y) * _frame.uv.w;

        //if( _frame.maxSize.x <= 0.f || _frame.maxSize.y <= 0.f )
        //{
        //    _frame.maxSize = _frame.size;
        //}

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
	//////////////////////////////////////////////////////////////////////////
}
