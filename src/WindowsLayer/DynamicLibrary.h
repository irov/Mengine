#	pragma once

#	include "Interface/WindowsLayerInterface.h"

#	include "WindowsIncluder.h"

namespace Menge
{
	class DynamicLibrary 
		: public DynamicLibraryInterface
	{
	public:
		DynamicLibrary( ServiceProviderInterface * _serviceProvider, const WString & _name );
		~DynamicLibrary();

	public:
		bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    public:
        void destroy() override;

	private:
		ServiceProviderInterface * m_serviceProvider;

		WString m_name;
		HINSTANCE m_hInstance;
	};
};
