#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "OpenGLRenderHeaderES.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class OpenGLRenderFragmentShaderES
		: public RenderFragmentShaderInterface
	{
	public:
		OpenGLRenderFragmentShaderES();
		~OpenGLRenderFragmentShaderES();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;
	
	public:
		const ConstString & getName() const override;

    public:
		bool initialize( const ConstString & _name, const void * _source, size_t _size, bool _isCompile );

	public:
		void attach( GLuint _program );

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;

		GLuint m_shaderId;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderFragmentShaderES> OpenGLRenderFragmentShaderESPtr;
}	// namespace Menge
