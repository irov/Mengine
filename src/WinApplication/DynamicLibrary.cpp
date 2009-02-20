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
		StringW strW = StringConversion::s_UTF8ToWChar( m_name );
		m_hInst = static_cast<hInstance>( LoadLibrary( strW.c_str() ) );
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
    void* DynamicLibrary::getSymbol( const String& strName ) const
	{
		return static_cast<void*>(GetProcAddress( m_hInst, strName.c_str() ));
	}
	//////////////////////////////////////////////////////////////////////////
}
