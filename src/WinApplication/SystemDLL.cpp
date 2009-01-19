#	include "SystemDLL.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
WinSystemDLL::WinSystemDLL(const Menge::String& _filename )
: m_filename( _filename )
{
#ifdef _DEBUG
	Menge::String filename_d( _filename + "_d" );
	m_hInstance = LoadLibraryA( filename_d.c_str() );
#else
	m_hInstance = LoadLibraryA( _filename.c_str() );
#endif

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
WinSystemDLL::~WinSystemDLL()
{
	//printf("free library - %s \n", m_filename.c_str() );
	if( FreeLibrary( (HMODULE) m_hInstance ) == 0 )
	{
		assert( !"FreeLibrary failure!" );
	}
}
