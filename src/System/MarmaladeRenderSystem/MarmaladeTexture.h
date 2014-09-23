#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/CacheInterface.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
	class MarmaladeTexture 
		: public RenderImageInterface
	{
	public:
		MarmaladeTexture();
		~MarmaladeTexture();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider, GLuint _uid, ERenderImageMode _mode, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type, bool _isRenderTarget );

	public:
        void * lock( size_t * _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;
	
	public:
		ERenderImageMode getMode() const override;

		uint32_t getHWWidth() const override;
		uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
		uint32_t getHWDepth() const override;

		PixelFormat getHWPixelFormat() const override;

	public:
		void _destroy() override;

	public: 
		bool isRenderTarget() const;
		GLuint getUId() const;
		
		void setMinFilter( GLenum _minFilter );
        GLenum getMinFilter() const;

		void setMagFilter( GLenum _magFilter );
        GLenum getMagFilter() const;
		
		void setWrapS( GLenum _wrapS );
        GLenum getWrapS() const;

		void setWrapT( GLenum _wrapT );		
        GLenum getWrapT() const;

	private:
		ServiceProviderInterface * m_serviceProvider;

		GLuint m_uid;
		bool m_isRenderTarget;
		
		PixelFormat m_hwPixelFormat;

		ERenderImageMode m_mode;

		uint32_t m_hwWidth;
		uint32_t m_hwHeight;
        uint32_t m_hwChannels;

		GLenum m_minFilter;
		GLenum m_magFilter;
		GLenum m_wrapS;
		GLenum m_wrapT;

		GLint m_internalFormat;

		GLenum m_format;
		GLenum m_type;

		void * m_buffer;
		CacheBufferID m_bufferId;
	};

    typedef stdex::intrusive_ptr<MarmaladeTexture> OGLTexturePtr;
}	// namespace Menge
