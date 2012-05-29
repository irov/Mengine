#	include "ResourceImage.h"
#	include "BinParser.h"

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

		float hwWidth = (float)texture->getHWWidth();
		float hwHeight = (float)texture->getHWHeight();

		_frame.pow_scale.x = width / hwWidth;
		_frame.pow_scale.y = height / hwHeight;

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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const WString& _fileName, const ConstString& _codec ) const
	{
		//printf( "loadImageFrame %s\n", _fileName.c_str() );

		RenderTextureInterface* texture = RenderEngine::get()
			->loadTexture( _pakName, _fileName, _codec );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "Warning: resource '%s' can't load image file '%S'"
				, getName().c_str()
				, _fileName.c_str() 
				);

			return false;
		}

		bool res = this->prepareImageFrame_( _frame, texture );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrameCombineRGBAndAlpha_( ImageFrame & _frame, const ConstString& _pakName,  const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB , const ConstString & _codecAlpha  ) const
	{
		RenderTextureInterface* texture = RenderEngine::get()
			->loadTextureCombineRGBAndAlpha( _pakName, _fileNameRGB, _fileNameAlpha , _codecRGB ,_codecAlpha );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImage::loadImageFrameCombineRGBAndAlpha_: resource '%s' can't load image file  with alpha data '%S' and rgb data %S "
				, getName().c_str()
				, _fileNameAlpha.c_str() 
				, _fileNameRGB.c_str() 
				);

			return false;
		}

		bool res = prepareImageFrame_( _frame, texture );

		return res;
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
	void ResourceImage::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Filter_Value, m_filter );
		}
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
	ImageDecoderInterface * ResourceImage::createDecoder_(  FileInputStreamInterface * _file, const ConstString& _codec )
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
