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

        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction DynamicLibrary::getSymbol( const String& strName ) const
	{
        if( m_hInstance == NULL )
        {
            return (TDynamicLibraryFunction)NULL;
        }

		FARPROC proc = ::GetProcAddress( m_hInstance, strName.c_str() );

		return (TDynamicLibraryFunction)proc;
	}
	//////////////////////////////////////////////////////////////////////////
}
