#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Configuration.h"

#include "Kernel/StaticString.h"
#include "Kernel/SpinLock.h"

#ifndef MENGINE_SERVICE_PROVIDER_MAX_REQUIRED
#define MENGINE_SERVICE_PROVIDER_MAX_REQUIRED 32U
#endif

#ifndef MENGINE_SERVICE_PROVIDER_NAME_SIZE
#define MENGINE_SERVICE_PROVIDER_NAME_SIZE 64U
#endif

#ifndef MENGINE_SERVICE_PROVIDER_COUNT
#define MENGINE_SERVICE_PROVIDER_COUNT 128U
#endif

#ifndef MENGINE_SERVICE_PROVIDER_MAX_WAIT
#define MENGINE_SERVICE_PROVIDER_MAX_WAIT 128U
#endif

#ifndef MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT
#define MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT 512U
#endif

#ifndef MENGINE_SERVICE_PROVIDER_LEAVE_COUNT
#define MENGINE_SERVICE_PROVIDER_LEAVE_COUNT 512U
#endif

namespace Mengine
{
    class ServiceProvider
        : public ServiceProviderInterface
    {
    public:
        ServiceProvider();
        ~ServiceProvider() override;

    public:
        void setConfiguration( const Configuration & _configuration ) override;
        const Configuration & getConfiguration() const override;

    public:
        const bool * isExistServiceProvider( const Char * _name ) override;
        const bool * isAvailableServiceProvider( const Char * _name ) override;
        const bool * isInitializeServiceProvider( const Char * _name ) override;

    public:
        bool validateServices() const override;
        void dependencyService( const Char * _name, const Char * _dependency ) override;

    public:
        bool waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda ) override;
        bool leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda ) override;
        void unlinkService( const Char * _owner ) override;

    protected:
        const ServiceInterfacePtr & getService( const Char * _name ) const override;
        bool createService( FServiceProviderGenerator _generator, bool _safe, const DocumentInterfacePtr & _doc ) override;
        bool finalizeService( const Char * _name ) override;
        bool destroyService( const Char * _name ) override;
        void stopServices() override;
        void destroy() override;

    protected:
        struct ServiceDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> required[MENGINE_SERVICE_PROVIDER_MAX_REQUIRED];
            uint32_t required_count = 0;
            ServiceInterfacePtr service;
            bool exist = false;
            bool safe = false;
            bool available = false;
            bool initialize = false;
            bool requiring = false;
        };

        struct LeaveDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> owner;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            LambdaLeaveService lambda;
        };

        struct WaitDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> owner;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            LambdaWaitService lambda;
        };

    protected:
        ServiceDesc * findServiceDesc_( const Char * _name );
        const ServiceDesc * findServiceDesc_( const Char * _name ) const;
        const Char * findUninitializedRequiredService_( const ServiceDesc & _desc ) const;
        bool hasRequiredService_( const ServiceDesc & _desc, const Char * _name ) const;
        bool hasDependency_( const ServiceDesc & _desc, const Char * _name, bool * const _visited ) const;

    protected:
        ServiceInterfacePtr generateService_( FServiceProviderGenerator _generator, const DocumentInterfacePtr & _doc );
        void removeDependency_( const Char * _name );
        void initializeService_( ServiceDesc * const _desc, const DocumentInterfacePtr & _doc, bool * const _result );
        bool autoRegistration_( ServiceDesc * const _desc );
        void autoUnregistration_( ServiceDesc * const _desc );
        bool checkWaits_( const ServiceInterfacePtr & _service );
        void deferredRequiredInitialize_( ServiceDesc * const _desc, const DocumentInterfacePtr & _doc, bool * const _result );
        void markServiceFinalized_( ServiceDesc * const _desc );
        ServiceInterfacePtr detachService_( ServiceDesc * const _desc );

    protected:
        struct DependencyDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> dependency;
        };

        DependencyDesc m_dependencies[MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT];
        uint32_t m_dependenciesCount;

        ServiceDesc m_services[MENGINE_SERVICE_PROVIDER_COUNT];
        uint32_t m_servicesCount;

        LeaveDesc m_leaves[MENGINE_SERVICE_PROVIDER_LEAVE_COUNT];
        uint32_t m_leaveCount;

        WaitDesc m_waits[MENGINE_SERVICE_PROVIDER_MAX_WAIT];
        uint32_t m_waitsCount;

        SpinLock m_spinLockServices;
        SpinLock m_spinLockDependencies;
        SpinLock m_spinLockLeaves;
        SpinLock m_spinLockWaits;

        Configuration m_configuration;

#if defined(MENGINE_DEBUG)
        const Char * m_initializeServiceName;
#endif
    };
}