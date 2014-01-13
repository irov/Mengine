#	include "DynamicLibrary.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::DynamicLibrary( const WString & _name )
		: m_name(_name)
		, m_hInstance(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::~DynamicLibrary()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DynamicLibrary::load()
	{
		m_hInstance = ::LoadLibrary( m_name.c_str() );

		if( m_hInstance == NULL )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void DynamicLibrary::destroy()
	{
		::FreeLibrary( m_hInstance );
		m_hInstance = NULL;

        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction DynamicLibrary::getSymbol( const char * _name ) const
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
