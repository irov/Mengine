#   include "DX9RenderVertexShader.h"

#	include "DX9ErrorHelper.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderVertexShader::DX9RenderVertexShader()
		: m_serviceProvider( nullptr )
		, m_shader( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & DX9RenderVertexShader::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderVertexShader::initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const MemoryInterfacePtr & _memory, bool _isCompile )
	{
		(void)_isCompile;

		m_serviceProvider = _serviceProvider;

		m_name = _name;

		m_memory = _memory;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderVertexShader::compile( IDirect3DDevice9 * _pD3DDevice )
	{
		const DWORD * dx_source = m_memory->getMemory();

		IF_DXCALL( m_serviceProvider, _pD3DDevice, CreateVertexShader, (dx_source, &m_shader) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderVertexShader::enable( IDirect3DDevice9 * _pD3DDevice )
	{
		IF_DXCALL( m_serviceProvider, _pD3DDevice, SetVertexShader, (m_shader) )
		{
			return false;
		}

		return true;
	}
}