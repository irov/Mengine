#include "Interface/ServiceProviderInterface.h"

#include "Config/Export.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DLL)
extern "C"
{
    MENGINE_DLL_EXPORT Mengine::ServiceProviderInterface * initializeMengine();
    MENGINE_DLL_EXPORT bool bootstrapMengine();
    MENGINE_DLL_EXPORT void finalizeMengine();
}
#else
    Mengine::ServiceProviderInterface * initializeMengine();
    bool bootstrapMengine();
    void finalizeMengine();
#endif
