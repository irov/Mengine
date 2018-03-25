#include "DX9RenderFragmentShader.h"

#include "DX9ErrorHelper.h"

#include "Logger/Logger.h"

#include "d3dx9.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	DX9RenderFragmentShader::DX9RenderFragmentShader()
		: m_shader( nullptr )
        , m_compile( false )
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
	bool DX9RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
	{
		m_name = _name;

		m_memory = _memory;
        m_compile = _compile;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderFragmentShader::compile( IDirect3DDevice9 * _pD3DDevice )
	{
        if( m_compile == false )
        {
            LPCSTR shader_source_data = m_memory->getMemory();
            size_t shader_source_size = m_memory->getSize();


            LPD3DXBUFFER ppShader;
            LPD3DXBUFFER ppErrorMsgs;
            HRESULT hr = D3DXCompileShader( shader_source_data, shader_source_size, NULL, NULL, "main", "ps_3_0", D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &ppShader, &ppErrorMsgs, NULL );

            if( hr != S_OK )
            {
                const Char * errorText = (const Char *)ppErrorMsgs->GetBufferPointer();

                LOGGER_ERROR( "DX9RenderFragmentShader::compile shader '%s' invalid compile:\n%s"
                    , m_name.c_str()
                    , errorText
                );

                return false;
            }

            const DWORD * shader_compile_data = (const DWORD *)ppShader->GetBufferPointer();
            //SIZE_T shader_compile_size = pVSBlob->GetBufferSize();

            IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, &m_shader) )
            {
                return false;
            }
        }
        else
        {
            const DWORD * shader_compile_data = m_memory->getMemory();

            IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, &m_shader) )
            {
                return false;
            }
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderFragmentShader::enable( IDirect3DDevice9 * _pD3DDevice )
	{
		IF_DXCALL( _pD3DDevice, SetPixelShader, (m_shader) )
		{
			return false;
		}

		return true;
	}
}