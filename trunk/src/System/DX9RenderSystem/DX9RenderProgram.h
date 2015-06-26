#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderProgram
		: public RenderProgramInterface
	{
	public:
		DX9RenderProgram();
		~DX9RenderProgram();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() override;

	public:
		bool initialize( const RenderShaderInterfacePtr & _vertexShader, const RenderShaderInterfacePtr & _fragmentShader );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		RenderShaderInterfacePtr m_vertexShader;
		RenderShaderInterfacePtr m_fragmentShader;
    };
}