#include "Interface/ServiceProviderInterface.h"

#include "Config/Export.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
    typedef Mengine::ServiceProviderInterface * (*FAPI_MengineCreate)(void);
    typedef Mengine::ServiceProviderInterface * (*FAPI_MengineGet)(void);
    typedef bool (*FAPI_MengineBootstrap)(void);
    typedef bool (*FAPI_MengineRun)(void);
    typedef void (*FAPI_MengineFinalize)(void);
#else
    extern "C" Mengine::ServiceProviderInterface * API_MengineCreate();
    extern "C" Mengine::ServiceProviderInterface * API_MengineGet();
    extern "C" bool API_MengineBootstrap();
    extern "C" bool API_MengineRun();
    extern "C" void API_MengineFinalize();
#endif