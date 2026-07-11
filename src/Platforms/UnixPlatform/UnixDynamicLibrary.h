#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/StaticString.h"

namespace Mengine
{
    class UnixDynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( UnixDynamicLibrary );

    public:
        UnixDynamicLibrary();
        ~UnixDynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load();
        void unload();

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    protected:
        StaticString<MENGINE_MAX_PATH> m_name;
        void * m_instance;
    };

    typedef IntrusivePtr<UnixDynamicLibrary, DynamicLibraryInterface> UnixDynamicLibraryPtr;
}
