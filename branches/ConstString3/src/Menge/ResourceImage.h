#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class TextureInterface;
	class FileInputStreamInterface;
	class ImageDecoderInterface;


	class ResourceImage
		: public ResourceReference
	{
	public:
		ResourceImage();
	public:
		struct ImageFrame
		{
			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv_image;
			mt::vec4f uv;
			mt::vec2f pow_scale;
			bool isAlpha;
			TextureInterface* texture;
			bool wrapX;
			bool wrapY;
		};

		virtual const mt::vec2f & getMaxSize() const = 0;
		virtual const mt::vec2f & getSize() const = 0;
		virtual const mt::vec2f & getOffset() const = 0;
		virtual const mt::vec4f & getUV() const = 0;
		virtual const mt::vec4f & getUVImage() const = 0;

		virtual bool isAlpha() const = 0;

		virtual TextureInterface* getTexture() const = 0;
		virtual const ConstString & getFileName() const = 0;
		virtual const ConstString & getCodecType() const = 0;

		virtual bool getWrapX() const;
		virtual bool getWrapY() const;
		//virtual size_t getCount() const;
	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool validImageFrame_( const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrameCombineRGBAndAlpha_( ImageFrame& _frame, const ConstString& _pakName, const ConstString& _fileNameRGB, const ConstString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;
		bool prepareImageFrame_( ImageFrame & _frame, TextureInterface * texture ) const;
		ImageDecoderInterface * createDecoder_( FileInputStreamInterface * _stream,  const ConstString& _codec );
		FileInputStreamInterface * createStream_( const ConstString& _pakName, const ConstString & _filename );

	public:
		static ConstString s_getImageCodec( const ConstString & _filename );
		
	protected:
		size_t m_filter;
	};
}
