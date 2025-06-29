#include "EnvironmentScriptEmbedding.h"

#include "Interface/EnvironmentServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentUserName( pybind::kernel_interface * _kernel )
        {
            Char userName[MENGINE_ENVIRONMENT_USER_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getUserName( userName );

            PyObject * py_userName = _kernel->string_from_char( userName );

            return py_userName;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentDeviceModel( pybind::kernel_interface * _kernel )
        {
            Char deviceModel[MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getDeviceModel( deviceModel );

            PyObject * py_deviceModel = _kernel->string_from_char( deviceModel );

            return py_deviceModel;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentDeviceLanguage( pybind::kernel_interface * _kernel )
        {
            Char deviceLanguage[MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getDeviceLanguage( deviceLanguage );

            PyObject * py_deviceLanguage = _kernel->string_from_char( deviceLanguage );

            return py_deviceLanguage;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentOSFamily( pybind::kernel_interface * _kernel )
        {
            Char osFamily[MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getOSFamily( osFamily );

            PyObject * py_osFamily = _kernel->string_from_char( osFamily );

            return py_osFamily;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentOSVersion( pybind::kernel_interface * _kernel )
        {
            Char osVersion[MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getOSVersion( osVersion );

            PyObject * py_osVersion = _kernel->string_from_char( osVersion );

            return py_osVersion;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentBundleId( pybind::kernel_interface * _kernel )
        {
            Char bundleId[MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getBundleId( bundleId );

            PyObject * py_bundleId = _kernel->string_from_char( bundleId );

            return py_bundleId;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentInstallId( pybind::kernel_interface * _kernel )
        {
            Char installId[MENGINE_ENVIRONMENT_INSTALLID_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getInstallId( installId );

            PyObject * py_installId = _kernel->string_from_char( installId );

            return py_installId;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * getEnvironmentInstallVersion( pybind::kernel_interface * _kernel )
        {
            Char installVersion[MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME + 1] = {'\0'};
            ENVIRONMENT_SERVICE()
                ->getInstallVersion( installVersion );

            PyObject * py_installVersion = _kernel->string_from_char( installVersion );

            return py_installVersion;
        }
        //////////////////////////////////////////////////////////////////////////
        static int64_t getEnvironmentInstallTimestamp( pybind::kernel_interface * _kernel )
        {
            MENGINE_UNUSED( _kernel );

            int64_t installTimestamp = ENVIRONMENT_SERVICE()
                ->getInstallTimestamp();

            return installTimestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        static int64_t getEnvironmentInstallRND( pybind::kernel_interface * _kernel )
        {
            MENGINE_UNUSED( _kernel );

            int64_t installRND = ENVIRONMENT_SERVICE()
                ->getInstallRND();

            return installRND;
        }
        //////////////////////////////////////////////////////////////////////////
        static int64_t getEnvironmentSessionIndex( pybind::kernel_interface * _kernel )
        {
            MENGINE_UNUSED( _kernel );

            int64_t sessionIndex = ENVIRONMENT_SERVICE()
                ->getSessionIndex();

            return sessionIndex;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    EnvironmentScriptEmbedding::EnvironmentScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EnvironmentScriptEmbedding::~EnvironmentScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EnvironmentScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_kernel( _kernel, "getEnvironmentUserName", &Detail::getEnvironmentUserName );
        pybind::def_function_kernel( _kernel, "getEnvironmentDeviceModel", &Detail::getEnvironmentDeviceModel );
        pybind::def_function_kernel( _kernel, "getEnvironmentDeviceLanguage", &Detail::getEnvironmentDeviceLanguage );
        pybind::def_function_kernel( _kernel, "getEnvironmentOSFamily", &Detail::getEnvironmentOSFamily );
        pybind::def_function_kernel( _kernel, "getEnvironmentOSVersion", &Detail::getEnvironmentOSVersion );
        pybind::def_function_kernel( _kernel, "getEnvironmentBundleId", &Detail::getEnvironmentBundleId );
        pybind::def_function_kernel( _kernel, "getEnvironmentInstallId", &Detail::getEnvironmentInstallId );
        pybind::def_function_kernel( _kernel, "getEnvironmentInstallVersion", &Detail::getEnvironmentInstallVersion );
        pybind::def_function_kernel( _kernel, "getEnvironmentInstallTimestamp", &Detail::getEnvironmentInstallTimestamp );
        pybind::def_function_kernel( _kernel, "getEnvironmentInstallRND", &Detail::getEnvironmentInstallRND );
        pybind::def_function_kernel( _kernel, "getEnvironmentSessionIndex", &Detail::getEnvironmentSessionIndex );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EnvironmentScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "getEnvironmentUserName", nullptr );
        _kernel->remove_from_module( "getEnvironmentDeviceModel", nullptr );
        _kernel->remove_from_module( "getEnvironmentDeviceLanguage", nullptr );
        _kernel->remove_from_module( "getEnvironmentOSFamily", nullptr );
        _kernel->remove_from_module( "getEnvironmentOSVersion", nullptr );
        _kernel->remove_from_module( "getEnvironmentBundleId", nullptr );
        _kernel->remove_from_module( "getEnvironmentSessionId", nullptr );
        _kernel->remove_from_module( "getEnvironmentInstallId", nullptr );
        _kernel->remove_from_module( "getEnvironmentInstallVersion", nullptr );
        _kernel->remove_from_module( "getEnvironmentInstallTimestamp", nullptr );
        _kernel->remove_from_module( "getEnvironmentInstallRND", nullptr );
        _kernel->remove_from_module( "getEnvironmentSessionIndex", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

