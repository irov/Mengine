#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include "WindowsIncluder.h"

namespace Menge
{
	class DynamicLibrary 
		: public DynamicLibraryInterface
	{
	public:
		DynamicLibrary( const WString& _name );
		~DynamicLibrary();

	public:
		void load() override;
        void unload() override;

	public:
		const WString& getName() const override;

	public:
        TDynamicLibraryFunction getSymbol( const String& _name ) const override;

	private:
		WString m_name;
		HINSTANCE m_hInstance;
	};
};
