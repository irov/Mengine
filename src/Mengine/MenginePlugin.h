#include "Interface/ServiceProviderInterface.h"

#include "Config/Export.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DLL)
extern "C"
{
    MENGINE_DLL_EXPORT Mengine::ServiceProviderInterface * API_MengineCreate();
    MENGINE_DLL_EXPORT bool API_MengineBootstrap();
    MENGINE_DLL_EXPORT bool API_MengineRun();
    MENGINE_DLL_EXPORT void API_MengineFinalize();
}
#else
    Mengine::ServiceProviderInterface * API_MengineCreate();
    bool API_MengineBootstrap();
    bool API_MengineRun();
    void API_MengineFinalize();
#endif
