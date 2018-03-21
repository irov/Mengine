#   pragma once

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderVertexShader.h"
#include "DX9RenderFragmentShader.h"

#include <d3d9.h>

namespace Mengine
{
	class DX9RenderProgram
		: public ServantBase<RenderProgramInterface>
	{
	public:
		DX9RenderProgram();
		~DX9RenderProgram();

	public:
		const ConstString & getName() const override;

	public:
        RenderVertexAttributeInterfacePtr getVertexAttribute() const override;
		RenderVertexShaderInterfacePtr getVertexShader() const override;
		RenderFragmentShaderInterfacePtr getFragmentShader() const override;

	public:
		bool initialize( const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute );

	public:
		bool compile( IDirect3DDevice9 * _pD3DDevice );

	public:
		bool enable( IDirect3DDevice9 * _pD3DDevice );

	public:
		void bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix );
		
	protected:
		ConstString m_name;

		DX9RenderVertexShaderPtr m_vertexShader;
		DX9RenderFragmentShaderPtr m_fragmentShader;
        RenderVertexAttributeInterfacePtr m_vertexAttribute;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DX9RenderProgram> DX9RenderProgramPtr;
}