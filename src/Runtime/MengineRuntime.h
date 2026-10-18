#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Configuration.h"

#include "Config/Export.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::ServiceProviderInterface * (*FAPI_MengineCreate)( const Mengine::Configuration * _configuration );
    typedef Mengine::ServiceProviderInterface * (*FAPI_MengineGet)();
    typedef bool (*FAPI_MengineBootstrap)();
    typedef bool (*FAPI_MengineRun)();
    typedef void (*FAPI_MengineFinalize)();
    //////////////////////////////////////////////////////////////////////////
#else
    //////////////////////////////////////////////////////////////////////////
    extern "C" Mengine::ServiceProviderInterface * API_MengineCreate( const Mengine::Configuration * _configuration );
    extern "C" Mengine::ServiceProviderInterface * API_MengineGet();
    extern "C" bool API_MengineBootstrap();
    extern "C" bool API_MengineRun();
    extern "C" void API_MengineFinalize();
    //////////////////////////////////////////////////////////////////////////
#endif