#	include "MarmaladeRenderTextureES1.h"

#	include "MarmaladeRenderErrorES1.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderTextureES1::MarmaladeRenderTextureES1()
		: m_serviceProvider(nullptr)
		, m_uid(0)
		, m_hwMipmaps(0)
		, m_hwWidth(0)
		, m_hwHeight(0)
        , m_hwChannels(0)
		, m_hwPixelFormat(PF_UNKNOWN)
		, m_internalFormat(GL_RGB)
		, m_format(GL_RGB)
		, m_type(GL_UNSIGNED_BYTE)
		, m_wrapS(GL_CLAMP_TO_EDGE)
		, m_wrapT(GL_CLAMP_TO_EDGE)
		, m_minFilter(GL_LINEAR)
		, m_magFilter(GL_LINEAR)
		, m_mode(ERIM_NORMAL)
		, m_lockLevel(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderTextureES1::~MarmaladeRenderTextureES1()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderTextureES1::initialize( ServiceProviderInterface * _serviceProvider, GLuint _uid, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
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
    }
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode MarmaladeRenderTextureES1::getMode() const
	{
		return m_mode;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeRenderTextureES1::getHWWidth() const 
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeRenderTextureES1::getHWHeight() const 
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MarmaladeRenderTextureES1::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderTextureES1::getHWDepth() const
	{
		return 1;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat MarmaladeRenderTextureES1::getHWPixelFormat() const 
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    void * MarmaladeRenderTextureES1::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		MemoryCacheBufferInterfacePtr lockMemory = MEMORY_SERVICE( m_serviceProvider )
			->createMemoryCacheBuffer();

		if( lockMemory == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeTexture::lock invalid create cache buffer"
				);

			return nullptr;
		}

		void * memory = lockMemory->cacheMemory( size, "MarmaladeTexture::lock" );

		if( memory == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
				, size
				, _level
				, miplevel_width
				, miplevel_height
				, m_hwChannels
				, m_hwPixelFormat
				);

			return nullptr;
		}

		m_lockMemory = lockMemory;
		
		*_pitch = size / miplevel_height;
		
		m_lockLevel = _level;

		return memory;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::unlock( uint32_t _level )
	{		
		GLCALL( m_serviceProvider, glBindTexture, ( GL_TEXTURE_2D, m_uid ) );

		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		GLuint textureMemorySize = Helper::getTextureMemorySize(miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat);

		LOGGER_INFO(m_serviceProvider)("MarmaladeTexture::unlock l %d w %d d %d"
			, _level
			, miplevel_width
			, miplevel_height
			, textureMemorySize
			);

		switch( m_internalFormat )
		{
		case GL_ETC1_RGB8_OES:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			{				
				void * memory = m_lockMemory->getMemory();

				IF_GLCALL( m_serviceProvider, glCompressedTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, memory) )
				{
					LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
						, _level
						, miplevel_width
						, miplevel_height
						, m_internalFormat
						, m_hwPixelFormat
						, textureMemorySize
						);
				}
			}break;
		default:
			{
				void * memory = m_lockMemory->getMemory();

				IF_GLCALL( m_serviceProvider, glTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, m_format, m_type, memory) )
				{
					LOGGER_ERROR(m_serviceProvider)("MarmaladeTexture::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
						, _level
						, miplevel_width
						, miplevel_height
						, m_internalFormat
						, m_format
						, m_type
						, m_hwPixelFormat
						);
				}
			}break;
		}

		m_lockMemory = nullptr;

		m_lockLevel = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::_destroy()
	{
		GLCALL( m_serviceProvider, glDeleteTextures, ( 1, &m_uid ) );
	}
	//////////////////////////////////////////////////////////////////////////
	GLuint MarmaladeRenderTextureES1::getUId() const
	{
		return m_uid;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeRenderTextureES1::getMinFilter() const
	{
		return m_minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::setMinFilter( GLenum _minFilter )
	{
		m_minFilter = _minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeRenderTextureES1::getMagFilter() const
	{
		return m_magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::setMagFilter( GLenum _magFilter )
	{
		m_magFilter = _magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeRenderTextureES1::getWrapS() const
	{
		return m_wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::setWrapS( GLenum _wrapS )
	{
		m_wrapS = _wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum MarmaladeRenderTextureES1::getWrapT() const
	{
		return m_wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderTextureES1::setWrapT( GLenum _wrapT )
	{
		m_wrapT = _wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
