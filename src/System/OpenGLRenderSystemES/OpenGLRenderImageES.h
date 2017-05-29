#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "OpenGLRenderHeaderES.h"

namespace Menge
{
	class OpenGLRenderImageES 
		: public RenderImageInterface
	{
	public:
		OpenGLRenderImageES();
		~OpenGLRenderImageES();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type );

	public:
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect& _rect, bool _readOnly ) override;
		void unlock( uint32_t _level ) override;
	
	public:
		ERenderImageMode getMode() const override;

		uint32_t getHWWidth() const override;
		uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
		uint32_t getHWDepth() const override;

		PixelFormat getHWPixelFormat() const override;

	public: 
		GLuint getUId() const;

	private:
		ServiceProviderInterface * m_serviceProvider;

		GLuint m_uid;
		
		PixelFormat m_hwPixelFormat;

		ERenderImageMode m_mode;

		uint32_t m_hwMipmaps;
		uint32_t m_hwWidth;
		uint32_t m_hwHeight;
        uint32_t m_hwChannels;

		GLint m_internalFormat;

		GLenum m_format;
		GLenum m_type;
				
		MemoryCacheBufferInterfacePtr m_lockMemory;
		uint32_t m_lockLevel;
	};

	typedef stdex::intrusive_ptr<OpenGLRenderImageES> OpenGLRenderImageESPtr;
}	// namespace Menge
