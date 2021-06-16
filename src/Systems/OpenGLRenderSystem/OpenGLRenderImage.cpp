#include "OpenGLRenderImage.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderImageLockedFactoryStorage.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"

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
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_lockFirst( false )
        , m_pow2( false )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::~OpenGLRenderImage()
    {
        MENGINE_ASSERTION_FATAL( m_uid == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
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

        m_width = _width;
        m_height = _height;
        m_hwMipmaps = _mipmaps;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_pow2 = Helper::isTexturePow2( _width ) == true && Helper::isTexturePow2( _height ) == true;
        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        if( this->create() == false )
        {
            LOGGER_ERROR( "invalid gen texture for size %u:%u PF %u"
                , _width
                , _height
                , _format
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::finalize()
    {
        this->release();

        m_renderImageProvider = nullptr;
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
    bool OpenGLRenderImage::getUpscalePow2() const
    {
        return m_upscalePow2;
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
    void OpenGLRenderImage::unbind( uint32_t _stage )
    {
#ifdef MENGINE_RENDER_OPENGL_ES
        GLCALL( glActiveTexture, (GL_TEXTURE0 + _stage) );
#else
        GLCALL( glActiveTexture_, (GL_TEXTURE0 + _stage) );
#endif

        GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );
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
            OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getRenderSystemExtention();

            extension->deleteTexture( m_uid );

            m_uid = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::create()
    {
        MENGINE_ASSERTION_FATAL( m_uid == 0 );

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getRenderSystemExtention();

        GLuint tuid = extension->genTexture();

        if( tuid == 0 )
        {
            return false;
        }

        m_uid = tuid;

        m_lockFirst = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr OpenGLRenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwPixelFormat );

        OpenGLRenderImageLockedPtr imageLocked = OpenGLRenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        size_t pitch = size / miplevel_height;

        imageLocked->initialize( size, pitch, _rect );

        return imageLocked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        if( _successful == false )
        {
            return true;
        }

        OpenGLRenderImageLocked * imageLocked = _locked.getT<OpenGLRenderImageLocked *>();

        const Rect & lockedRect = imageLocked->getLockedRect();

        size_t pitch;
        void * buffer = imageLocked->getBuffer( &pitch );

        GLCALL( glBindTexture, (GL_TEXTURE_2D, m_uid) );

        LOGGER_INFO( "render", "l %u r %u:%u-%u:%u"
            , _level
            , lockedRect.left
            , lockedRect.top
            , lockedRect.right
            , lockedRect.bottom
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
                if( lockedRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    GLuint textureMemorySize = Helper::getTextureMemorySize( miplevel_hwwidth, miplevel_hwheight, m_hwPixelFormat );
#ifdef MENGINE_RENDER_OPENGL_ES
                    IF_GLCALL( glCompressedTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, buffer) )
#else
                    IF_GLCALL( glCompressedTexImage2D_, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, buffer) )
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
                if( lockedRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, buffer) )
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
                    uint32_t miplevel_xoffset = lockedRect.left >> _level;
                    uint32_t miplevel_yoffset = lockedRect.top >> _level;
                    uint32_t miplevel_width = (lockedRect.right - lockedRect.left) >> _level;
                    uint32_t miplevel_height = (lockedRect.bottom - lockedRect.top) >> _level;

                    if( m_lockFirst == true )
                    {
                        IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, nullptr) )
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

                    IF_GLCALL( glTexSubImage2D, (GL_TEXTURE_2D, _level, miplevel_xoffset, miplevel_yoffset, miplevel_width, miplevel_height, m_format, m_type, buffer) )
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

        return successful;
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
    GLuint OpenGLRenderImage::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImage::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::onRenderRestore()
    {
        if( this->create() == false )
        {
            return false;
        }

        if( m_renderImageProvider == nullptr )
        {
            return true;
        }

        RenderImageLoaderInterfacePtr loader = m_renderImageProvider->getLoader( MENGINE_DOCUMENT_FACTORABLE );

        if( loader->load( RenderImageInterfacePtr( this ) ) == false )
        {
            LOGGER_ERROR( "Invalid decode image"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
