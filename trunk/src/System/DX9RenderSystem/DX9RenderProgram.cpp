#   include "DX9RenderProgram.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	DX9RenderProgram::DX9RenderProgram()
		: m_serviceProvider( nullptr )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	DX9RenderProgram::~DX9RenderProgram()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void DX9RenderProgram::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * DX9RenderProgram::getServiceProvider()
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderProgram::initialize( const RenderShaderInterfacePtr & _vertexShader, const RenderShaderInterfacePtr & _fragmentShader )
	{ 
		m_vertexShader = _vertexShader;
		m_fragmentShader = _fragmentShader;

		return true;
	}
}