#	include "DynamicLibrary.h"

#	include "windows.h"

#	include "StringConversion.h"

namespace Menge
{
	DynamicLibrary::DynamicLibrary( const String& _name )
		: m_name( _name )
		, m_hInst( NULL )
	{}
	//////////////////////////////////////////////////////////////////////////
	DynamicLibrary::~DynamicLibrary()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DynamicLibrary::load()
	{
		StringW strW = StringConversion::utf8ToWChar( m_name );

		hInstance h = LoadLibrary( strW.c_str() );

		m_hInst = h;
	}
	//////////////////////////////////////////////////////////////////////////
    void DynamicLibrary::unload()
	{
		FreeLibrary( m_hInst );
	}
	//////////////////////////////////////////////////////////////////////////
	const String& DynamicLibrary::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
    TFunctionPtr DynamicLibrary::getSymbol( const String& strName ) const
	{
		return (TFunctionPtr)(GetProcAddress( m_hInst, strName.c_str() ));
	}
	//////////////////////////////////////////////////////////////////////////
}
