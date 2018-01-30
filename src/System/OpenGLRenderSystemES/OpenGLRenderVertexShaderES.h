#	pragma once

#	include "Interface/RenderSystemInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "OpenGLRenderHeaderES.h"

#   include "Core/ServantBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class OpenGLRenderVertexShaderES
		: public ServantBase<RenderVertexShaderInterface>
	{
	public:
		OpenGLRenderVertexShaderES();
		~OpenGLRenderVertexShaderES();

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
		ConstString m_name;
        
        MemoryInterfacePtr m_memory;

		GLuint m_shaderId;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderVertexShaderES> OpenGLRenderVertexShaderESPtr;
}	// namespace Menge
