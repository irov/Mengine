#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define VERTEX_POSITION_ARRAY			0
#	define VERTEX_COLOR_ARRAY				1
#	define VERTEX_UV0_ARRAY				2
#	define VERTEX_UV1_ARRAY				3
#	define VERTEX_UV2_ARRAY				4
#	define VERTEX_UV3_ARRAY				5
#	define VERTEX_UV4_ARRAY				6
#	define VERTEX_UV5_ARRAY				7
#	define VERTEX_UV6_ARRAY				8
#	define VERTEX_UV7_ARRAY				9
	//////////////////////////////////////////////////////////////////////////
	class MarmaladeShader
		: public RenderShaderInterface
	{
	public:
		MarmaladeShader();
		~MarmaladeShader();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() override;
	
	public:
		const ConstString & getName() const override;

    public:
		bool initialize( const ConstString & _name, GLenum type, const void * _source, size_t _size, bool _isCompile );
		void finalize();

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
	typedef stdex::intrusive_ptr<MarmaladeShader> MarmaladeShaderPtr;
}	// namespace Menge
