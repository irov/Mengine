#	pragma once

#	include "Interface/WindowsLayerInterface.h"

#	include "WindowsIncluder.h"

namespace Menge
{
	class DynamicLibrary 
		: public DynamicLibraryInterface
	{
	public:
		DynamicLibrary( const WString & _name );
		~DynamicLibrary();

	public:
		bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const char * _name ) const override;

    public:
        void destroy() override;

	private:
		WString m_name;
		HINSTANCE m_hInstance;
	};
};
