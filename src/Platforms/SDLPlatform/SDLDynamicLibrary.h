#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    class SDLDynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
    public:
        SDLDynamicLibrary();
        ~SDLDynamicLibrary() override;

    public:
        void setName( const String & _name );
        const String & getName() const;

    public:
        bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        String m_name;
        void * m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDynamicLibrary> SDLDynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
