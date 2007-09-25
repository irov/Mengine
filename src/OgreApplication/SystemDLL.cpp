#	include "SystemDLL.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <assert.h>


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SystemDLL::SystemDLL( const std::string &_filename )
	: m_filename( _filename )
	{
		m_hInstance = LoadLibrary( _filename.c_str() );

		if( m_hInstance )
		{
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
		else
		{
			assert(!"can't load DLL");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLL::~SystemDLL()
	{
		printf("free library - %s \n", m_filename.c_str() );
		if( FreeLibrary( (HMODULE) m_hInstance ) == 0 )
		{
			assert( !"FreeLibrary failure!" );
		}
	}
}