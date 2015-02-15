#	include "MarmaladeTexture.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTexture::MarmaladeTexture()
		: m_serviceProvider(nullptr)
		, m_uid(0)
		, m_hwMipmaps(0)
		, m_hwWidth(0)
		, m_hwHeight(0)
        , m_hwChannels(0)
		, m_hwPixelFormat(PF_UNKNOWN)
		, m_internalFormat(0)
		, m_format(0)
		, m_type(0)
		, m_wrapS(0)
		, m_wrapT(0)
		, m_minFilter(0)
		, m_magFilter(0)
		, m_mode(ERIM_NORMAL)
		, m_lockMemory(nullptr)
		, m_lockBufferId(INVALID_CACHE_BUFFER_ID)
		, m_lockLevel(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeTexture::~MarmaladeTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeTexture::initialize( ServiceProviderInterface * _serviceProvider, GLuint _uid, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
		m_serviceProvider = _serviceProvider;
		        
		m_uid = _uid;
		m_mode = _mode;
		m_hwMipmaps = _mipmaps;
        m_hwWidth = _width;
        m_hwHeight = _height;
        m_hwChannels = _channels;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;		

		glBindTexture( GL_TEXTURE_2D, m_uid );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_minFilter );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_magFilter );

        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
    }
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode MarmaladeTexture::getMode() const
	{
		return m_mode;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTexture::getHWWidth() const 
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTexture::getHWHeight() const 
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeTexture::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeTexture::getHWDepth() const
	{
		return 1;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat MarmaladeTexture::getHWPixelFormat() const 
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    void * MarmaladeTexture::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		void * memory;
		CacheBufferID lockBufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( size, &memory, "MarmaladeTexture::lock" );

		if( lockBufferId == INVALID_CACHE_BUFFER_ID )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
				, size
				, _level
				, miplevel_width
				, miplevel_height
				, m_hwChannels
				, m_hwPixelFormat
				);

			return nullptr;
		}

		m_lockBufferId = lockBufferId;
		m_lockMemory = memory;
		
		*_pitch = size / miplevel_height;
		
		m_lockLevel = _level;

		return m_lockMemory;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::unlock( uint32_t _level )
	{		
		glBindTexture( GL_TEXTURE_2D, m_uid );

		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		GLuint textureMemorySize = Helper::getTextureMemorySize(miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat);

		LOGGER_INFO(m_serviceProvider)("MarmaladeTexture::unlock l %d w %d d %d"
			, _level
			, miplevel_width
			, miplevel_height
			, textureMemorySize
			);

		//if( m_mode == ERIM_NORMAL )
		{
			switch( m_internalFormat )
			{
			case GL_ETC1_RGB8_OES:
			case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
			case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
				{				
					glCompressedTexImage2D( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, m_lockMemory );
				}break;
			default:
				{
					glTexImage2D( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, m_format, m_type, m_lockMemory );				
				}break;
			}
		}
		//else if( m_mode == ERIM_DYNAMIC )
		//{
		//	switch( m_internalFormat )
		//	{
		//	case GL_ETC1_RGB8_OES:
		//	case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		//	case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
		//		{			
		//			glCompressedTexImage2D( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, m_lockMemory );
		//			glCompressedTexSubImage2D( GL_TEXTURE_2D, m_lockLevel, 0, 0, miplevel_width, miplevel_height, m_internalFormat, textureMemorySize, m_lockMemory );
		//		}break;
		//	default:
		//		{
		//			glTexSubImage2D( GL_TEXTURE_2D, m_lockLevel, 0, 0, miplevel_width, miplevel_height, m_format, m_type, m_lockMemory );				
		//		}break;
		//	}
		//}
		//else
		{
			//Empty
		}

		GLenum gl_err = glGetError();

		if( gl_err != GL_NO_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock gl error %d param l %d w %d h %d i %d p %d"
				, gl_err
				, _level
				, miplevel_width
				, miplevel_height
				, m_internalFormat
				, m_hwPixelFormat
				);
		}

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_lockBufferId );

		m_lockLevel = 0;
		m_lockBufferId = INVALID_CACHE_BUFFER_ID;
		m_lockMemory = nullptr;		
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeTexture::_destroy()
	{
		glDeleteTextures( 1, &m_uid );
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
