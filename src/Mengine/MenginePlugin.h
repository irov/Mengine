#include "Interface/ServiceProviderInterface.h"

#include "Config/Export.h"

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
    typedef Mengine::ServiceProviderInterface * (*FAPI_MengineCreate)(void);
    typedef bool (*FAPI_MengineBootstrap)(void);
    typedef bool (*FAPI_MengineRun)(void);
    typedef void (*FAPI_MengineFinalize)(void);
#else
    MENGINE_EXPORT_API Mengine::ServiceProviderInterface * API_MengineCreate();
    MENGINE_EXPORT_API bool API_MengineBootstrap();
    MENGINE_EXPORT_API bool API_MengineRun();
    MENGINE_EXPORT_API void API_MengineFinalize();
#endif
