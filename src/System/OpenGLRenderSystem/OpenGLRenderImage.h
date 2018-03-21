#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "OpenGLRenderExtension.h"

#include "Core/ServantBase.h"

namespace Mengine
{
    class OpenGLRenderImage
        : public ServantBase<RenderImageInterface>
    {
    public:
        OpenGLRenderImage();
        ~OpenGLRenderImage();

    public:
        bool initialize( ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type );
        void finalize();
        
    public:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        void release();
        bool reload();
        
    public:
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect& _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;
    
    public:
        ERenderImageMode getMode() const override;

        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;

        PixelFormat getHWPixelFormat() const override;
        
    public:
        void _destroy() override;

    public: 
        GLuint getUId() const;
        
        void setMinFilter( GLenum _minFilter );
        GLenum getMinFilter() const;

        void setMagFilter( GLenum _magFilter );
        GLenum getMagFilter() const;
        
        void setWrapS( GLenum _wrapS );
        GLenum getWrapS() const;

        void setWrapT( GLenum _wrapT );     
        GLenum getWrapT() const;

    protected:
        RenderImageProviderInterfacePtr m_renderImageProvider;

        GLuint m_uid = 0;
        
        PixelFormat m_hwPixelFormat = PF_UNKNOWN;

        ERenderImageMode m_mode = ERIM_NORMAL;

        uint32_t m_hwMipmaps = 0;
        uint32_t m_hwWidth = 0;
        uint32_t m_hwHeight = 0;
        uint32_t m_hwChannels = 0;

        GLenum m_minFilter = GL_LINEAR;
        GLenum m_magFilter = GL_LINEAR;
        GLenum m_wrapS = GL_CLAMP_TO_EDGE;
        GLenum m_wrapT = GL_CLAMP_TO_EDGE;

        GLint m_internalFormat = GL_RGB;

        GLenum m_format = GL_RGB;
        GLenum m_type = GL_UNSIGNED_BYTE;

        MemoryInterfacePtr m_lockMemory;
        Rect m_lockRect;
        uint32_t m_lockLevel = 0;
        bool m_lockFirst;

        bool m_pow2 = false;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderImage> OpenGLRenderImagePtr;
}
