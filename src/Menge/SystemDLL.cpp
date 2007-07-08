#	include "SystemDLL.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <assert.h>


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SystemDLLImpl::SystemDLLImpl( const std::string &_filename )
	{
		m_hInstance = LoadLibrary( _filename.c_str() );

		if( m_hInstance == NULL )
		{
			assert( !"DLL load failure!" );
			if( FreeLibrary( (HMODULE) m_hInstance ) == 0 )
			{
				assert( !"FreeLibrary failure!" );
			}
		}

		m_init = (FInterface)GetProcAddress( (HMODULE) m_hInstance, "initInterfaceSystem" );

		if( m_init == NULL )
		{
			assert( !"Init Function failure!" );
		}

		m_fini = (FInterface)GetProcAddress( (HMODULE) m_hInstance, "releaseInterfaceSystem" );

		if( m_fini == NULL )
		{
			assert( !"Release Function failure!" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLLImpl::~SystemDLLImpl()
	{
		if( FreeLibrary( (HMODULE) m_hInstance ) == 0 )
		{
			assert( !"FreeLibrary failure!" );
		}
	}
}