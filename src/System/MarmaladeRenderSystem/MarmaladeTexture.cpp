#	include "MarmaladeTexture.h"

#	include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTexture::MarmaladeTexture()
		: m_serviceProvider(nullptr)
		, m_uid(0)
		, m_hwWidth(0)
		, m_hwHeight(0)
        , m_hwChannels(0)
		, m_hwPixelFormat(PF_UNKNOWN)
		, m_internalFormat(0)
		, m_format(0)
		, m_type(0)
		, m_isRenderTarget(false)		
		, m_wrapS(0)
		, m_wrapT(0)
		, m_minFilter(0)
		, m_magFilter(0)
		, m_mode(ERIM_NORMAL)
		, m_lock(nullptr)
		, m_bufferId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTexture::~MarmaladeTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeTexture::initialize( ServiceProviderInterface * _serviceProvider, GLuint _uid, ERenderImageMode _mode, size_t _width, size_t _height, size_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type, bool _isRenderTarget )
    {
		m_serviceProvider = _serviceProvider;

        m_uid = _uid;
        m_hwWidth = _width;
        m_hwHeight = _height;
        m_hwChannels = _channels;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;
        m_isRenderTarget = _isRenderTarget;
		m_mode = _mode;
    }
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode MarmaladeTexture::getMode() const
	{
		return m_mode;
	}
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeTexture::getHWWidth() const 
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeTexture::getHWHeight() const 
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeTexture::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeTexture::getHWDepth() const
	{
		return 1;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat MarmaladeTexture::getHWPixelFormat() const 
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    void * MarmaladeTexture::lock( int * _pitch, const Rect & _rect, bool _readOnly )
    {
		size_t size = Helper::getTextureMemorySize( m_hwWidth, m_hwHeight, m_hwChannels, 1, m_hwPixelFormat );

		m_bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( size, &m_lock, "MarmaladeTexture::lock" );
		
		*_pitch = size / m_hwHeight;

		return m_lock;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::unlock()
	{
		glBindTexture( GL_TEXTURE_2D, m_uid );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_minFilter );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_magFilter );

        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );

		switch( m_internalFormat )
	    {
			case GL_ETC1_RGB8_OES:
			case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
			case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			{
				GLuint size = Helper::getTextureMemorySize(m_hwWidth, m_hwHeight, m_hwChannels, 1, m_hwPixelFormat);
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_hwWidth, m_hwHeight, 0, size, m_lock);				
			}break;
			case GL_RGB:
			{
				glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_hwWidth, m_hwHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_lock );
				
			}break;
			default:
			{
				glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_hwWidth, m_hwHeight, 0, m_format, m_type, m_lock );				
			}break;
		}

		GLenum gl_err = glGetError();

		if( gl_err != GL_NO_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock gl error %d param %d %d %d"
				, gl_err
				, m_hwWidth
				, m_hwHeight
				, m_hwPixelFormat
				);
		}

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_bufferId );

		m_bufferId = 0;
		m_lock = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::_destroy()
	{
		glDeleteTextures( 1, &m_uid );
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeTexture::isRenderTarget() const
	{
		return m_isRenderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	GLuint MarmaladeTexture::getUId() const
	{
		return m_uid;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTexture::getMinFilter() const
	{
		return m_minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::setMinFilter( GLenum _minFilter )
	{
		m_minFilter = _minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTexture::getMagFilter() const
	{
		return m_magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::setMagFilter( GLenum _magFilter )
	{
		m_magFilter = _magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTexture::getWrapS() const
	{
		return m_wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::setWrapS( GLenum _wrapS )
	{
		m_wrapS = _wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeTexture::getWrapT() const
	{
		return m_wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::setWrapT( GLenum _wrapT )
	{
		m_wrapT = _wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
