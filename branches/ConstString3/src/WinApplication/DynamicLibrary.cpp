#	include "DynamicLibrary.h"

#	include "windows.h"

#	include "StringConversion.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::DynamicLibrary( const WString& _name )
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
		m_hInstance = LoadLibrary( m_name.c_str() );

		if( m_hInstance == NULL )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void DynamicLibrary::unload()
	{
		FreeLibrary( m_hInstance );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& DynamicLibrary::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction DynamicLibrary::getSymbol( const String& strName ) const
	{
		FARPROC proc = GetProcAddress( m_hInstance, strName.c_str() );

		return (TDynamicLibraryFunction)proc;
	}
	//////////////////////////////////////////////////////////////////////////
}
