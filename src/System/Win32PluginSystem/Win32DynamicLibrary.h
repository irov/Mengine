#pragma once

#include "Interface/WindowsLayerInterface.h"

#include "Core/ServantBase.h"

#   include "Utils/WIN32/WindowsIncluder.h"

namespace Mengine
{
	class Win32DynamicLibrary 
		: public ServantBase<DynamicLibraryInterface>
	{
	public:
		Win32DynamicLibrary();
		~Win32DynamicLibrary();

	public:
		void setName(const WString & _name);
		const WString & getName() const;

	public:
		bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

	protected:
		WString m_name;
		HINSTANCE m_hInstance;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Win32DynamicLibrary> Win32DynamicLibraryPtr;
	//////////////////////////////////////////////////////////////////////////
};
