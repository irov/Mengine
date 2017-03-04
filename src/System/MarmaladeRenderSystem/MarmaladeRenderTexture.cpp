#	include "MarmaladeRenderTexture.h"

#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderTexture::MarmaladeRenderTexture()
		: m_serviceProvider( nullptr )
		, m_uid( 0 )
		, m_hwMipmaps( 0 )
		, m_hwWidth( 0 )
		, m_hwHeight( 0 )
		, m_hwChannels( 0 )
		, m_hwPixelFormat( PF_UNKNOWN )
		, m_internalFormat( GL_RGB )
		, m_format( GL_RGB )
		, m_type( GL_UNSIGNED_BYTE )
		, m_mode( ERIM_NORMAL )
		, m_lockLevel( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderTexture::~MarmaladeRenderTexture()
	{
		if( m_uid != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteTextures, (1, &m_uid) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderTexture::initialize( ServiceProviderInterface * _serviceProvider, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
	{
		m_serviceProvider = _serviceProvider;

		switch( _internalFormat )
		{
		case GL_ETC1_RGB8_OES:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			{
				if( _width != _height )
				{
					LOGGER_ERROR( m_serviceProvider )("MarmaladeTexture::initialize not square texture %d:%d"
						, _width
						, _height
						);

					return false;
				}
			}break;
		}

		GLuint tuid = 0;
		GLCALL( m_serviceProvider, glGenTextures, (1, &tuid) );

		if( tuid == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeTexture::initialize invalid gen texture for size %d:%d channel %d PF %d"
				, _width
				, _height
				, _channels
				, _format
				);

			return false;
		}

		m_uid = tuid;
		m_mode = _mode;
		m_hwMipmaps = _mipmaps;
		m_hwWidth = _width;
		m_hwHeight = _height;
		m_hwChannels = _channels;
		m_hwPixelFormat = _pixelFormat;
		m_internalFormat = _internalFormat;
		m_format = _format;
		m_type = _type;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode MarmaladeRenderTexture::getMode() const
	{
		return m_mode;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderTexture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderTexture::getHWHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderTexture::getHWChannels() const
	{
		return m_hwChannels;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderTexture::getHWDepth() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat MarmaladeRenderTexture::getHWPixelFormat() const
	{
		return m_hwPixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer MarmaladeRenderTexture::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
	{
		if( m_lockMemory != nullptr )
		{
			return nullptr;
		}

		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		MemoryCacheBufferInterfacePtr lockMemory = MEMORY_SERVICE( m_serviceProvider )
			->createMemoryCacheBuffer();

		if( lockMemory == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderTexture::lock invalid create cache buffer"
				);

			return nullptr;
		}

		void * memory = lockMemory->cacheMemory( size, "MarmaladeRenderTexture::lock" );

		if( memory == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
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
	void MarmaladeRenderTexture::unlock( uint32_t _level )
	{
		GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, m_uid) );

		uint32_t miplevel_width = m_hwWidth >> _level;
		uint32_t miplevel_height = m_hwHeight >> _level;

		GLuint textureMemorySize = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		LOGGER_INFO( m_serviceProvider )("MarmaladeRenderTexture::unlock l %d w %d d %d"
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
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			{
				void * memory = m_lockMemory->getMemory();

				IF_GLCALL( m_serviceProvider, glCompressedTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, memory) )
				{
					LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderTexture::unlock glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
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
					LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderTexture::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
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
	GLuint MarmaladeRenderTexture::getUId() const
	{
		return m_uid;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
