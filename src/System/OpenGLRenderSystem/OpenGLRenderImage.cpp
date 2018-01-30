#	include "OpenGLRenderImage.h"

#	include "OpenGLRenderError.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::OpenGLRenderImage()
        : m_uid( 0 )
        , m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwChannels( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_mode( ERIM_NORMAL )
        , m_lockLevel( 0 )
        , m_lockFirst( false )
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

        m_lockFirst = true;

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
    void OpenGLRenderImage::release()
    {
        GLCALL( m_serviceProvider, glDeleteTextures, (1, &m_uid) );

        m_uid = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::reload()
    {
        GLuint tuid = 0;
        GLCALL( m_serviceProvider, glGenTextures, (1, &tuid) );

        m_uid = tuid;

        if( m_renderImageProvider == nullptr )
        {
            return true;
        }

        RenderImageLoaderInterfacePtr loader = m_renderImageProvider->getLoader();

        if( loader->load( this ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::createTexture Invalid decode image"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer OpenGLRenderImage::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        (void)_readOnly;

        if( m_lockMemory != nullptr )
        {
            return nullptr;
        }

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

        MemoryInterfacePtr lockMemory = MEMORY_SERVICE( m_serviceProvider )
            ->createMemoryCacheBuffer();

        if( lockMemory == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLTexture::lock invalid create cache buffer");

            return nullptr;
        }

        void * memory = lockMemory->newMemory( size, __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLTexture::lock invalid cache memory %d (l %d w %d h %d c %d f %d)"
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
        m_lockRect = _rect;

        *_pitch = size / miplevel_height;

        m_lockLevel = _level;

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::unlock( uint32_t _level, bool _successful )
    {
        if( _successful == false )
        {
            m_lockMemory = nullptr;
            m_lockLevel = 0;

            return true;
        }

        GLCALL( m_serviceProvider, glEnable, (GL_TEXTURE_2D) );
        GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, m_uid) );


        LOGGER_INFO( m_serviceProvider )("OpenGLRenderImage::unlock l %d r %d:%d-%d:%d"
            , _level
            , m_lockRect.left
            , m_lockRect.top
            , m_lockRect.right
            , m_lockRect.bottom
            );

        void * memory = m_lockMemory->getMemory();

        bool successful = true;

        if( m_lockRect.full( m_hwWidth, m_hwHeight ) == true )
        {
            uint32_t miplevel_hwwidth = m_hwWidth >> _level;
            uint32_t miplevel_hwheight = m_hwHeight >> _level;

            switch( m_internalFormat )
            {
            case GL_ETC1_RGB8_OES:
            case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#ifdef _WIN32
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
#endif
                {
                    GLuint textureMemorySize = Helper::getTextureMemorySize( miplevel_hwwidth, miplevel_hwheight, m_hwChannels, 1, m_hwPixelFormat );

                    IF_GLCALL( m_serviceProvider, mglCompressedTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0, textureMemorySize, memory) )
                    {
                        LOGGER_ERROR( m_serviceProvider )("OpenGLRenderImage::unlock glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
                            , _level
                            , miplevel_hwwidth
                            , miplevel_hwheight
                            , m_internalFormat
                            , m_hwPixelFormat
                            , textureMemorySize
                            );

                        successful = false;
                    }
                }break;
            default:
                {
                    IF_GLCALL( m_serviceProvider, glTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0, m_format, m_type, memory) )
                    {
                        LOGGER_ERROR( m_serviceProvider )("OpenGLRenderImage::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                            , _level
                            , miplevel_hwwidth
                            , miplevel_hwheight
                            , m_internalFormat
                            , m_format
                            , m_type
                            , m_hwPixelFormat
                            );

                        successful = false;
                    }
                }break;
            }
        }
        else
        {
            switch( m_internalFormat )
            {
            case GL_ETC1_RGB8_OES:
            case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#ifdef _WIN32
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
#endif
                {
                    successful = false;
                }break;
            default:
                {
                    uint32_t miplevel_xoffset = m_lockRect.left >> _level;
                    uint32_t miplevel_yoffset = m_lockRect.top >> _level;
                    uint32_t miplevel_width = (m_lockRect.right - m_lockRect.left) >> _level;
                    uint32_t miplevel_height = (m_lockRect.bottom - m_lockRect.top) >> _level;

                    if( m_lockFirst == true )
                    {
                        uint32_t miplevel_hwwidth = m_hwWidth >> _level;
                        uint32_t miplevel_hwheight = m_hwHeight >> _level;

                        IF_GLCALL( m_serviceProvider, glTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0, m_format, m_type, nullptr) )
                        {
                            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderImage::unlock glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                                , _level
                                , miplevel_hwwidth
                                , miplevel_hwheight
                                , m_internalFormat
                                , m_format
                                , m_type
                                , m_hwPixelFormat
                                );

                            successful = false;
                        }

                        m_lockFirst = false;
                    }

                    IF_GLCALL( m_serviceProvider, glTexSubImage2D, (GL_TEXTURE_2D, m_lockLevel
                        , miplevel_xoffset
                        , miplevel_yoffset
                        , miplevel_width
                        , miplevel_height
                        , m_format
                        , m_type
                        , memory) )
                    {
                        LOGGER_ERROR( m_serviceProvider )("OpenGLRenderImage::unlock glTexSubImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                            , _level
                            , miplevel_width
                            , miplevel_height
                            , m_internalFormat
                            , m_format
                            , m_type
                            , m_hwPixelFormat
                            );

                        successful = false;
                    }
                }break;
            }
        }

        m_lockMemory = nullptr;
        m_lockLevel = 0;
        m_lockFirst = false;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::_destroy()
    {
        GLCALL( m_serviceProvider, glDeleteTextures, (1, &m_uid) );
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
