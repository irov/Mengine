#	include "Win32DynamicLibrary.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32DynamicLibrary::Win32DynamicLibrary()
		: m_hInstance(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32DynamicLibrary ::~Win32DynamicLibrary()
	{
		if( m_hInstance != NULL )
		{
			::FreeLibrary( m_hInstance );
			m_hInstance = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32DynamicLibrary::setName(const WString & _name)
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Win32DynamicLibrary::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32DynamicLibrary::load()
	{
		m_hInstance = ::LoadLibrary( m_name.c_str() );

		if( m_hInstance == NULL )
		{
			DWORD le = GetLastError();

			LOGGER_ERROR("DynamicLibrary::load %ls error code %d"
				, m_name.c_str()
				, le
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	TDynamicLibraryFunction Win32DynamicLibrary::getSymbol( const Char * _name ) const
	{
        if( m_hInstance == NULL )
        {
            return NULL;
        }

		FARPROC proc = ::GetProcAddress( m_hInstance, _name );

        TDynamicLibraryFunction dlfunc = (TDynamicLibraryFunction)proc;

		return dlfunc;
	}
	//////////////////////////////////////////////////////////////////////////
}
