#pragma once

#include "Interface/PluginInterface.h"

#include "Kernel/Factorable.h"

#include "Config/String.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32DynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
    public:
        Win32DynamicLibrary();
        ~Win32DynamicLibrary() override;

    public:
        void setName( const String & _name );
        const String & getName() const;

    public:
        bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    protected:
        String m_name;
        HINSTANCE m_hInstance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DynamicLibrary> Win32DynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
