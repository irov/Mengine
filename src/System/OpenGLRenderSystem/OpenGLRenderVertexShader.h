#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "OpenGLRenderHeader.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class OpenGLRenderVertexShader
		: public RenderVertexShaderInterface
	{
	public:
		OpenGLRenderVertexShader();
		~OpenGLRenderVertexShader();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;
	
	public:
		const ConstString & getName() const override;

    public:
		bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory );
        bool compile();

	public:
		void attach( GLuint _program );

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;
        MemoryInterfacePtr m_memory;

		GLuint m_shaderId;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderVertexShader> OpenGLRenderVertexShaderPtr;
}	// namespace Menge
