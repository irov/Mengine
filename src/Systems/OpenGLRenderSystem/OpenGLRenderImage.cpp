#include "OpenGLRenderImage.h"

#include "Interface/MemoryServiceInterface.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderError.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::OpenGLRenderImage()
        : m_uid( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwChannels( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_lockLevel( 0 )
        , m_lockFirst( false )
        , m_pow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::~OpenGLRenderImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
        MENGINE_ASSERTION_FATAL( _width != 0 );
        MENGINE_ASSERTION_FATAL( _height != 0 );

        switch( _internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            {
                if( _width != _height )
                {
                    LOGGER_ERROR( "not square texture %d:%d"
                        , _width
                        , _height
                    );

                    return false;
                }
            }break;
        default:
            break;
        }

        GLuint tuid = 0;
        GLCALL( glGenTextures, (1, &tuid) );

        if( tuid == 0 )
        {
            LOGGER_ERROR( "invalid gen texture for size %d:%d channel %d PF %d"
                , _width
                , _height
                , _channels
                , _format
            );

            return false;
        }

        m_uid = tuid;

        m_width = _width;
        m_height = _height;
        m_hwMipmaps = _mipmaps;
        m_hwWidth = Helper::getTexturePOW2( _width );
        m_hwHeight = Helper::getTexturePOW2( _height );
        m_hwChannels = _channels;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_pow2 = Helper::isTexturePOW2( _width ) && Helper::isTexturePOW2( _height );

        m_lockFirst = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::finalize()
    {
        this->release();

        m_renderImageProvider = nullptr;
        m_lockMemory = nullptr;
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
    EPixelFormat OpenGLRenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer OpenGLRenderImage::getRenderImageExtention()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::bind( uint32_t _stage )
    {
#ifdef MENGINE_RENDER_OPENGL_ES
        GLCALL( glActiveTexture, (GL_TEXTURE0 + _stage) );
#else
        GLCALL( glActiveTexture_, (GL_TEXTURE0 + _stage) );
#endif

        GLCALL( glBindTexture, (GL_TEXTURE_2D, m_uid) );
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
        if( m_uid != 0 )
        {
            GLCALL( glDeleteTextures, (1, &m_uid) );

            m_uid = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::reload()
    {
        GLuint tuid = 0;
        GLCALL( glGenTextures, (1, &tuid) );

        m_uid = tuid;

        m_lockFirst = true;

        if( m_renderImageProvider == nullptr )
        {
            return true;
        }

        RenderImageLoaderInterfacePtr loader = m_renderImageProvider->getLoader();

        if( loader->load( RenderImageInterfacePtr( this ) ) == false )
        {
            LOGGER_ERROR( "Invalid decode image"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer OpenGLRenderImage::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        if( m_lockMemory != nullptr )
        {
            return nullptr;
        }

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory (l %u w %u h %u c %u f %u)"
            , _level
            , miplevel_width
            , miplevel_height
            , m_hwChannels
            , m_hwPixelFormat
        );

        void * buffer = memory->newBuffer( size );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid new memory %zu (l %u w %u h %u c %u f %u)"
            , size
            , _level
            , miplevel_width
            , miplevel_height
            , m_hwChannels
            , m_hwPixelFormat
        );

        m_lockMemory = memory;
        m_lockRect = _rect;

        *_pitch = size / miplevel_height;

        m_lockLevel = _level;

        return buffer;
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

#ifndef MENGINE_RENDER_OPENGL_ES
        GLCALL( glEnable, (GL_TEXTURE_2D) );
#endif
        GLCALL( glBindTexture, (GL_TEXTURE_2D, m_uid) );

        LOGGER_INFO( "l %d r %d:%d-%d:%d"
            , _level
            , m_lockRect.left
            , m_lockRect.top
            , m_lockRect.right
            , m_lockRect.bottom
        );

        bool successful = true;

        uint32_t miplevel_hwwidth = m_hwWidth >> _level;
        uint32_t miplevel_hwheight = m_hwHeight >> _level;

        switch( m_internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#ifdef MENGINE_PLATFORM_WINDOWS
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
#endif
            {
                if( m_lockRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    void * buffer = m_lockMemory->getBuffer();

                    GLuint textureMemorySize = Helper::getTextureMemorySize( miplevel_hwwidth, miplevel_hwheight, m_hwChannels, 1, m_hwPixelFormat );
#ifdef MENGINE_RENDER_OPENGL_ES
                    IF_GLCALL( glCompressedTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, buffer) )
#else
                    IF_GLCALL( glCompressedTexImage2D_, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, buffer) )
#endif
                    {
                        LOGGER_ERROR( "glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
                            , _level
                            , miplevel_hwwidth
                            , miplevel_hwheight
                            , m_internalFormat
                            , m_hwPixelFormat
                            , textureMemorySize
                        );

                        successful = false;
                    }
                }
                else
                {
                    successful = false;
                }
            }break;
        default:
            {
                void * buffer = m_lockMemory->getBuffer();

                if( m_lockRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, buffer) )
                    {
                        LOGGER_ERROR( "glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
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
                }
                else
                {
                    uint32_t miplevel_xoffset = m_lockRect.left >> _level;
                    uint32_t miplevel_yoffset = m_lockRect.top >> _level;
                    uint32_t miplevel_width = (m_lockRect.right - m_lockRect.left) >> _level;
                    uint32_t miplevel_height = (m_lockRect.bottom - m_lockRect.top) >> _level;

                    if( m_lockFirst == true )
                    {
                        IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, m_lockLevel, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, nullptr) )
                        {
                            LOGGER_ERROR( "glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
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
                    }

                    IF_GLCALL( glTexSubImage2D, (GL_TEXTURE_2D, m_lockLevel, miplevel_xoffset, miplevel_yoffset, miplevel_width, miplevel_height, m_format, m_type, buffer) )
                    {
                        LOGGER_ERROR( "glTexSubImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
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
                }
            }break;
        }

        m_lockFirst = false;

        m_lockMemory = nullptr;
        m_lockLevel = 0;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::_destroy()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderImage::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setMinFilter( GLenum _minFilter )
    {
        m_minFilter = _minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getMinFilter() const
    {
        return m_minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setMagFilter( GLenum _magFilter )
    {
        m_magFilter = _magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getMagFilter() const
    {
        return m_magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setWrapS( GLenum _wrapS )
    {
        m_wrapS = _wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getWrapS() const
    {
        return m_wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::setWrapT( GLenum _wrapT )
    {
        m_wrapT = _wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getWrapT() const
    {
        return m_wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
}
