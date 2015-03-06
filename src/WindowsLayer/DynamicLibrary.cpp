#	include "DynamicLibrary.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::DynamicLibrary( ServiceProviderInterface * _serviceProvider, const WString & _name )
		: m_serviceProvider(_serviceProvider)
		, m_name(_name)
		, m_hInstance(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::~DynamicLibrary()
	{
		if( m_hInstance != NULL )
		{
			::FreeLibrary( m_hInstance );
			m_hInstance = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DynamicLibrary::load()
	{
		m_hInstance = ::LoadLibrary( m_name.c_str() );

		if( m_hInstance == NULL )
		{
			DWORD le = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("DynamicLibrary::load %ls error code %d"
				, m_name.c_str()
				, le
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void DynamicLibrary::destroy()
	{
        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction DynamicLibrary::getSymbol( const String & _name ) const
	{
        if( m_hInstance == NULL )
        {
            return NULL;
        }

		FARPROC proc = ::GetProcAddress( m_hInstance, _name.c_str() );

        TDynamicLibraryFunction dlfunc = (TDynamicLibraryFunction)proc;

		return dlfunc;
	}
	//////////////////////////////////////////////////////////////////////////
}
