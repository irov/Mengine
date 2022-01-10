#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLDynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLDynamicLibrary );

    public:
        SDLDynamicLibrary();
        ~SDLDynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load();
        void unload();

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        StaticString<MENGINE_MAX_PATH> m_name;
        void * m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDynamicLibrary, DynamicLibraryInterface> SDLDynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
