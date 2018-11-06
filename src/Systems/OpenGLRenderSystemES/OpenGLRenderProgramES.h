#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "OpenGLRenderShaderES.h"
#	include "OpenGLRenderVertexShaderES.h"
#	include "OpenGLRenderFragmentShaderES.h"

#	include "OpenGLRenderHeaderES.h"

namespace Mengine
{
	class OpenGLRenderProgramES
		: public RenderProgramInterface
	{
	public:
		OpenGLRenderProgramES();
		~OpenGLRenderProgramES();

	public:
		const ConstString & getName() const override;

	public:
		RenderFragmentShaderInterfacePtr getFragmentShader() const override;
		RenderVertexShaderInterfacePtr getVertexShader() const override;

    public:
		bool initialize( const ConstString & _name, const OpenGLRenderVertexShaderESPtr & _vertexShader, const OpenGLRenderFragmentShaderESPtr & _fragmentShader, uint32_t _samplerCount );
        bool compile();

	public:
		void enable() const;
		void disable() const;

		void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const;
		void bindTexture( uint32_t _textureInd ) const;

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ConstString m_name;

		GLuint m_program;

		OpenGLRenderVertexShaderESPtr m_vertexShader;
		OpenGLRenderFragmentShaderESPtr m_fragmentShader;

		uint32_t m_samplerCount;
		
		mutable mt::mat4f m_mvpMat;

		int m_transformLocation;
		int m_samplerLocation[MENGINE_MAX_TEXTURE_STAGES];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderProgramES> OpenGLRenderProgramESPtr;
	//////////////////////////////////////////////////////////////////////////
}

