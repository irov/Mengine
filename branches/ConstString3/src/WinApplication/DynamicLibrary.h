#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include "WindowsIncluder.h"

namespace Menge
{
	class DynamicLibrary 
		: public DynamicLibraryInterface
	{
	public:
		DynamicLibrary( const String& _name );
		~DynamicLibrary();

	public:
		void load() override;
        void unload() override;
		const String& getName() const override;
        TDynamicLibraryFunction getSymbol( const String& _name ) const override;

	private:
		String m_name;
		HINSTANCE m_hInstance;
	};
};
