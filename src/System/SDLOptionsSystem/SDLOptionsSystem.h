#   pragma once

#   include "Interface/OptionsInterface.h"

#   include "Config/Char.h"

#   include <stdex/stl_list.h>

namespace Menge
{
    class SDLOptionsSystem
        : public ServiceBase<OptionsSystemInterface>
    {
    public:
        void setArgs( char ** argv, const int argc );
        bool getOptions( Char * _options, size_t _capacity ) const override;

    private:
        String  m_commandLine;
    };

#   define SDLOPTIONS_SERVICE( serviceProvider )\
    ((Menge::SDLOptionsSystem*)SERVICE_GET(serviceProvider, Menge::SDLOptionsSystem))
}
