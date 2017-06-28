#   pragma once

#   include "Interface/PluginInterface.h"

#	include "Core/ServantBase.h"

namespace Menge
{
    class SDLDynamicLibrary 
        : public ServantBase<DynamicLibraryInterface>
    {
    public:
        SDLDynamicLibrary();
        ~SDLDynamicLibrary();

    public:
        void setName( const String & _name );
        const String & getName() const;

    public:
        bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        String m_name;
        void* m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SDLDynamicLibrary> SDLDynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
