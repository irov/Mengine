#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class RenderTextureInterface;
	class FileInputStreamInterface;
	class ImageDecoderInterface;
    
    struct ImageFrame
    {
        RenderTextureInterface* texture;

        mt::vec2f maxSize;
        mt::vec2f size;
        mt::vec4f uv_image;
        mt::vec4f uv;
        mt::vec4f uv_scale;

        bool isAlpha;
        bool wrapX;
        bool wrapY;
    };

	class ResourceImage
		: public ResourceReference
	{
	public:
		ResourceImage();

    public:
        inline const ImageFrame & getImageFrame() const;

	public:        
		inline const mt::vec2f & getMaxSize() const;
		inline const mt::vec2f & getSize() const;
		inline const mt::vec4f & getUV() const;
		inline const mt::vec4f & getUVImage() const;

		inline bool isAlpha() const;
        inline bool getWrapX() const;
        inline bool getWrapY() const;
        
		inline RenderTextureInterface* getTexture() const;

    public:
        virtual bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) = 0; //FixMe

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
        ImageFrame m_imageFrame;
	};
    //////////////////////////////////////////////////////////////////////////
    inline const ImageFrame & ResourceImage::getImageFrame() const
    {
        return m_imageFrame;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_imageFrame.maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getSize() const
    {
        return m_imageFrame.size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUV() const
    {
        return m_imageFrame.uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVImage() const
    {
        return m_imageFrame.uv_image;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isAlpha() const
    {
        return m_imageFrame.isAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline RenderTextureInterface* ResourceImage::getTexture() const
    {
        return m_imageFrame.texture;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::getWrapX() const
    {
        return m_imageFrame.wrapX;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::getWrapY() const
    {
        return m_imageFrame.wrapY;
    }
}
