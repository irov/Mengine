#include "OpenGLRenderTargetTexture.h"

#include "Interface/RenderSystemInterface.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderErrorHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderTargetTexture::OpenGLRenderTargetTexture()
        : m_tuid( 0 )
        , m_fuid( 0 )
        , m_oldfuid( 0 )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_pow2( false )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderTargetTexture::~OpenGLRenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_tuid == 0 );
        MENGINE_ASSERTION_FATAL( m_fuid == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
        MENGINE_ASSERTION_FATAL( _width != 0 );
        MENGINE_ASSERTION_FATAL( _height != 0 );

        switch( _internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            {
                return false;
            }break;
        default:
            break;
        }

        m_width = _width;
        m_height = _height;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;

        float u = float( m_width ) / float( m_hwWidth );
        float v = float( m_height ) / float( m_hwHeight );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, v, u, 0.f ) );

        m_pow2 = Helper::isTexturePow2( _width ) == true && Helper::isTexturePow2( _height ) == true;
        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        if( this->create() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::finalize()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::create()
    {
        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint tuid = extension->genTexture();

        MENGINE_GLCALL( glBindTexture, (GL_TEXTURE_2D, tuid) );
        MENGINE_GLCALL( glTexImage2D, (GL_TEXTURE_2D, 0, GL_RGB, m_hwWidth, m_hwHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0) );

        m_tuid = tuid;

        GLuint fuid = extension->genFramebuffer();

        if( fuid == 0 )
        {
            LOGGER_ERROR( "invalid gen framebuffer" );

            extension->deleteTexture( m_tuid );
            m_tuid = 0;

            return false;
        }

        m_fuid = fuid;
        
        GLint oldFBO = 0;
        MENGINE_GLCALL( glGetIntegerv, (GL_FRAMEBUFFER_BINDING, &oldFBO) );

        MENGINE_GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, m_fuid) );


#if defined(MENGINE_RENDER_OPENGL_ES)
        MENGINE_GLCALL( glFramebufferTexture2D, (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tuid, 0) );
#else
        MENGINE_GLCALL( glFramebufferTexture, (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tuid, 0) );

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        MENGINE_GLCALL( glDrawBuffers, (1, DrawBuffers) );
#endif

        if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
        {
            MENGINE_GLERRORCHECK();

            extension->deleteTexture( m_tuid );
            m_tuid = 0;

            extension->deleteFramebuffer( m_fuid );
            m_fuid = 0;

            return false;
        }
        
        MENGINE_GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, oldFBO) );
        
        MENGINE_GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::release()
    {
        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        if( m_tuid != 0 )
        {
            extension->deleteTexture( m_tuid );
            m_tuid = 0;
        }

        if( m_fuid != 0 )
        {
            extension->deleteFramebuffer( m_fuid );
            m_fuid = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::reload()
    {
        MENGINE_ASSERTION_FATAL( m_tuid == 0 );
        MENGINE_ASSERTION_FATAL( m_fuid == 0 );

        if( this->create() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat OpenGLRenderTargetTexture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::getUpscalePow2() const
    {
        return m_upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::begin() const
    {
        GLint oldFBO = 0;
        MENGINE_GLCALL( glGetIntegerv, (GL_FRAMEBUFFER_BINDING, &oldFBO) );
        
        m_oldfuid = oldFBO;
        
        MENGINE_GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, m_fuid) );

        MENGINE_GLCALL( glViewport, (0, 0, m_width, m_height) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::end() const
    {
        MENGINE_GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, m_oldfuid) );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & OpenGLRenderTargetTexture::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderTargetTexture::getUID() const
    {
        return m_tuid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
