#   include "DX9RenderProgram.h"

#	include "DX9ErrorHelper.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	DX9RenderProgram::DX9RenderProgram()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	DX9RenderProgram::~DX9RenderProgram()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & DX9RenderProgram::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexShaderInterfacePtr DX9RenderProgram::getVertexShader() const
	{
		return m_vertexShader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderFragmentShaderInterfacePtr DX9RenderProgram::getFragmentShader() const
	{
		return m_fragmentShader;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderProgram::initialize( const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader )
	{ 
		m_name = _name;

		m_vertexShader = _vertexShader;
		m_fragmentShader = _fragmentShader;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderProgram::compile( IDirect3DDevice9 * _pD3DDevice )
	{
		(void)_pD3DDevice;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderProgram::enable( IDirect3DDevice9 * _pD3DDevice )
	{
		if( m_vertexShader != nullptr )
		{
			if( m_vertexShader->enable( _pD3DDevice ) == false )
			{
				return false;
			}
		}

		if( m_fragmentShader != nullptr )
		{
			if( m_fragmentShader->enable( _pD3DDevice ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderProgram::bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix )
	{
		mt::mat4f mvpMat = _worldMatrix * _viewMatrix * _projectionMatrix;
		mt::mat4f mvpMat_tr;
		mt::transpose_m4( mvpMat_tr, mvpMat );
		DXCALL( m_serviceProvider, _pD3DDevice, SetVertexShaderConstantF, (0, mvpMat_tr.buff(), 4) );

		//GLCALL( m_serviceProvider, glUniformMatrix4fv, (m_transformLocation, 1, GL_FALSE, m_mvpMat.buff()) );
	}
}