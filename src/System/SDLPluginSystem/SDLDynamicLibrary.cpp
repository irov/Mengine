#   include "SDLDynamicLibrary.h"

#   include "Logger/Logger.h"

#   include "SDL_loadso.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLDynamicLibrary::SDLDynamicLibrary( ServiceProviderInterface * _serviceProvider, const String & _name )
        : m_serviceProvider(_serviceProvider)
        , m_name(_name)
        , m_instance(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLDynamicLibrary::~SDLDynamicLibrary()
    {
        if (m_instance)
        {
            SDL_UnloadObject(m_instance);
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLDynamicLibrary::load()
    {
        m_instance = SDL_LoadObject(m_name.c_str());

        if (m_instance == nullptr)
        {
            LOGGER_ERROR(m_serviceProvider)("DynamicLibrary::load %ls failed"
                , m_name.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDynamicLibrary::destroy()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction SDLDynamicLibrary::getSymbol( const Char * _name ) const
    {
        if (m_instance)
        {
            void* proc = ::SDL_LoadFunction(m_instance, _name);

            TDynamicLibraryFunction dlfunc = reinterpret_cast<TDynamicLibraryFunction>(proc);

            return dlfunc;
        }
        else
        {
            return nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
