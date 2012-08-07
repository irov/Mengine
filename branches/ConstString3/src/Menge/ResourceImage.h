#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class RenderTextureInterface;
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
			RenderTextureInterface* texture;

			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv_image;
			mt::vec4f uv;
			mt::vec4f uv_scale;
			//mt::vec2f pow_offset;
			//mt::vec2f pow_scale;			

			bool isAlpha;
			bool wrapX;
			bool wrapY;
		};

		virtual const mt::vec2f & getMaxSize() const = 0;
		virtual const mt::vec2f & getSize() const = 0;
		virtual const mt::vec2f & getOffset() const = 0;
		virtual const mt::vec4f & getUV() const = 0;
		virtual const mt::vec4f & getUVImage() const = 0;

		virtual bool isAlpha() const = 0;

		virtual RenderTextureInterface* getTexture() const = 0;
		virtual const WString & getFileName() const = 0;
		virtual const ConstString & getCodecType() const = 0;
		virtual bool loadBuffer( unsigned char * _buffer, int _pitch ) = 0;
		virtual bool getWrapX() const;
		virtual bool getWrapY() const;
		//virtual size_t getCount() const;
	public:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const WString& _filename, const ConstString& _codec ) const;
		
		//bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;
		bool prepareImageFrame_( ImageFrame & _frame, RenderTextureInterface * texture ) const;

	protected:
		ImageDecoderInterface * createDecoder_( FileInputStreamInterface * _stream,  const ConstString& _codec ) const;

	public:
		static const ConstString & s_getImageCodec( const WString & _filename );
		
	protected:
		size_t m_filter;
	};
}
