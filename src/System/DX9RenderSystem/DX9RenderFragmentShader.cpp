#   include "DX9RenderFragmentShader.h"

#	include "DX9ErrorHelper.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	DX9RenderFragmentShader::DX9RenderFragmentShader()
		: m_shader( nullptr )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	DX9RenderFragmentShader::~DX9RenderFragmentShader()
	{
		if( m_shader != nullptr )
		{
			m_shader->Release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & DX9RenderFragmentShader::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _isCompile )
	{
		(void)_isCompile;

		m_name = _name;

		m_memory = _memory;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderFragmentShader::compile( IDirect3DDevice9 * _pD3DDevice )
	{
		const DWORD * dx_source = m_memory->getMemory();

		IF_DXCALL( m_serviceProvider, _pD3DDevice, CreatePixelShader, (dx_source, &m_shader) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderFragmentShader::enable( IDirect3DDevice9 * _pD3DDevice )
	{
		IF_DXCALL( m_serviceProvider, _pD3DDevice, SetPixelShader, (m_shader) )
		{
			return false;
		}

		return true;
	}
}