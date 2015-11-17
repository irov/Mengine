#	pragma once

#	include "Interface/WindowsLayerInterface.h"

#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#	ifndef NOMINMAX
#	define NOMINMAX
#	endif

#	pragma warning(push, 0) 
#	include <Windows.h>
#   include <WinUser.h>

#   include <shellapi.h>
#   include <shlobj.h>
#	pragma warning(pop) 

namespace Menge
{
	class Win32DynamicLibrary 
		: public DynamicLibraryInterface
	{
	public:
		Win32DynamicLibrary( ServiceProviderInterface * _serviceProvider, const WString & _name );
		~Win32DynamicLibrary();

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
