#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "MarmaladeRenderShader.h"
#	include "MarmaladeRenderVertexShader.h"
#	include "MarmaladeRenderFragmentShader.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
	class MarmaladeRenderProgram
		: public RenderProgramInterface
	{
	public:
		MarmaladeRenderProgram();
		~MarmaladeRenderProgram();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		const ConstString & getName() const override;

	public:
		RenderFragmentShaderInterfacePtr getFragmentShader() const override;
		RenderVertexShaderInterfacePtr getVertexShader() const override;

    public:
		bool initialize( const ConstString & _name, const MarmaladeRenderVertexShaderPtr & _vertexShader, const MarmaladeRenderFragmentShaderPtr & _fragmentShader, uint32_t _samplerCount );

	public:
		void enable() const;
		void disable() const;

		void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const;
		void bindTexture( uint32_t _textureInd ) const;

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;

		GLuint m_program;

		MarmaladeRenderVertexShaderPtr m_vertexShader;
		MarmaladeRenderFragmentShaderPtr m_fragmentShader;

		uint32_t m_samplerCount;
		
		mutable mt::mat4f m_mvpMat;

		int m_transformLocation;
		int m_samplerLocation[MENGE_MAX_TEXTURE_STAGES];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MarmaladeRenderProgram> MarmaladeProgramPtr;
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

