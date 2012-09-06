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
		inline const mt::vec2f & getMaxSize() const;
		inline const mt::vec2f & getSize() const;
		inline const mt::vec4f & getUV() const;
        inline const mt::vec4f & getUVScale() const;
		inline const mt::vec4f & getUVImage() const;

		inline bool isAlpha() const;
        inline bool getWrapX() const;
        inline bool getWrapY() const;
        
		inline RenderTextureInterface* getTexture() const;

    public:
        inline RenderTextureInterface* getTextureAlpha() const;
        inline const mt::vec4f & getUVAlpha() const;

    public:
        virtual bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) = 0; //FixMe

	protected:
		bool loadImageFrame_( const ConstString& _pakName, const WString& _filename, const ConstString& _codec );
		
		//bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		bool prepareImageFrame_( RenderTextureInterface * _texture );

	protected:
		ImageDecoderInterface * createDecoder_( FileInputStreamInterface * _stream,  const ConstString& _codec ) const;

	public:
		static const ConstString & s_getImageCodec( const WString & _filename );
		
	protected:
        RenderTextureInterface* m_texture;
        RenderTextureInterface* m_textureAlpha;

        mt::vec2f m_maxSize;
        mt::vec2f m_size;
        mt::vec4f m_uv_image;
        mt::vec4f m_uv;
        mt::vec4f m_uv_scale;

        mt::vec4f m_uv_alpha;

        bool m_isAlpha;
        bool m_wrapX;
        bool m_wrapY;
	};
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getMaxSize() const
    {
        return m_maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & ResourceImage::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVScale() const
    {
        return m_uv_scale;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVImage() const
    {
        return m_uv_image;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::isAlpha() const
    {
        return m_isAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline RenderTextureInterface* ResourceImage::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::getWrapX() const
    {
        return m_wrapX;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool ResourceImage::getWrapY() const
    {
        return m_wrapY;
    }
    //////////////////////////////////////////////////////////////////////////
    inline RenderTextureInterface * ResourceImage::getTextureAlpha() const
    {
        return m_textureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec4f & ResourceImage::getUVAlpha() const
    {
        return m_uv_alpha;
    }
}
