#	include "DynamicLibrary.h"

#	include "windows.h"

#	include "StringConversion.h"

namespace Menge
{
	DynamicLibrary::DynamicLibrary( const String& _name )
		: m_name(_name)
		, m_hInstance(NULL)
	{}
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::~DynamicLibrary()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DynamicLibrary::load()
	{
		StringW strW = StringConversion::utf8ToWChar( m_name );

		m_hInstance = LoadLibrary( strW.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
    void DynamicLibrary::unload()
	{
		FreeLibrary( m_hInstance );
	}
	//////////////////////////////////////////////////////////////////////////
	const String& DynamicLibrary::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
    TFunctionPtr DynamicLibrary::getSymbol( const String& strName ) const
	{
		return (TFunctionPtr)(GetProcAddress( m_hInstance, strName.c_str() ));
	}
	//////////////////////////////////////////////////////////////////////////
}
