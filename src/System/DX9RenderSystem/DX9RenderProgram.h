#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "DX9RenderVertexShader.h"
#	include "DX9RenderFragmentShader.h"

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
		const ConstString & getName() const override;

	public:
		RenderVertexShaderInterfacePtr getVertexShader() const override;
		RenderFragmentShaderInterfacePtr getFragmentShader() const override;

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader );

	public:
		bool compile( IDirect3DDevice9 * _pD3DDevice );

	public:
		bool enable( IDirect3DDevice9 * _pD3DDevice );

	public:
		void bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix );
		
	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;

		DX9RenderVertexShaderPtr m_vertexShader;
		DX9RenderFragmentShaderPtr m_fragmentShader;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DX9RenderProgram> DX9RenderProgramPtr;
}