#	include "OpenGLRenderImage.h"

#	include "OpenGLRenderError.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::OpenGLRenderImage()
        : m_uid(0)
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
        , m_pow2(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::~OpenGLRenderImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::initialize( ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {       
        switch( _internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            {
                if( _width != _height )
                {
                    LOGGER_ERROR( m_serviceProvider )("OpenGLTexture::initialize not square texture %d:%d"
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
            LOGGER_ERROR( m_serviceProvider )("OpenGLTexture::initialize invalid gen texture for size %d:%d channel %d PF %d"
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

        m_pow2 = Helper::isTexturePOW2( m_hwWidth ) && Helper::isTexturePOW2( m_hwHeight );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderImageMode OpenGLRenderImage::getMode() const
    {
        return m_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImage::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImage::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImage::getHWDepth() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PixelFormat OpenGLRenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::isPow2() const
    {
        return m_pow2;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & OpenGLRenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer OpenGLRenderImage::lock( size_t * _pitch, uint32_t _level, const Rect &, bool )
    {
        if (m_lockMemory != nullptr)
        {
            return nullptr;
        }

        uint32_t miplevel_width = m_hwWidth >> _level;
        uint32_t miplevel_height = m_hwHeight >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

        MemoryInterfacePtr lockMemory = MEMORY_SERVICE(m_serviceProvider)
            ->createMemoryCacheBuffer();

        if (lockMemory == nullptr)
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLTexture::lock invalid create cache buffer");

            return nullptr;
        }

        void * memory = lockMemory->newMemory( size, __FILE__, __LINE__ );

        if (memory == nullptr)
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
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
    void OpenGLRenderImage::unlock( uint32_t _level )
    {		
        GLCALL( m_serviceProvider, glBindTexture, ( GL_TEXTURE_2D, m_uid ) );

        uint32_t miplevel_width = m_hwWidth >> _level;
        uint32_t miplevel_height = m_hwHeight >> _level;

        GLuint textureMemorySize = Helper::getTextureMemorySize(miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat);

        LOGGER_INFO(m_serviceProvider)("OpenGLTexture::unlock l %d w %d d %d"
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

                IF_GLCALL( m_serviceProvider, glCompressedTexImage2D_, ( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, textureMemorySize, memory) )
                {
                    LOGGER_ERROR(m_serviceProvider)("OpenGLTexture::unlock glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
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

                IF_GLCALL( m_serviceProvider, glTexImage2D, ( GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_width, miplevel_height, 0, m_format, m_type, memory) )
                {
                    LOGGER_ERROR(m_serviceProvider)("OpenGLTexture::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
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
    void OpenGLRenderImage::_destroy()
    {
        GLCALL( m_serviceProvider, glDeleteTextures, ( 1, &m_uid ) );
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderImage::getUId() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getMinFilter() const
    {
        return m_minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setMinFilter( GLenum _minFilter )
    {
        m_minFilter = _minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getMagFilter() const
    {
        return m_magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setMagFilter( GLenum _magFilter )
    {
        m_magFilter = _magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getWrapS() const
    {
        return m_wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setWrapS( GLenum _wrapS )
    {
        m_wrapS = _wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getWrapT() const
    {
        return m_wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setWrapT( GLenum _wrapT )
    {
        m_wrapT = _wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
}   // namespace Menge
