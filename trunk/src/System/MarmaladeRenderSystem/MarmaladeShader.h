#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
	class MarmaladeShader
		: public RenderShaderInterface
	{
	public:
		MarmaladeShader();
		~MarmaladeShader();

    public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const char * _vsSrc, const char * _fsSrc );
		void finalize();

	public:
		void use(const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix) const;
		void bindTexture(unsigned int _textureInd, int _texture) const;

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		GLuint m_program;

		GLuint m_vertexShader;
		GLuint m_fragmentShader;
		
		mutable mt::mat4f m_mvpMat;

		int m_transformLocation;
		int m_samplerLocation[2];
	};
}	// namespace Menge
