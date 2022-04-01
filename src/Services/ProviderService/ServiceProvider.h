#pragma once

#include "Interface/ServiceProviderInterface.h"

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
        const bool * isExistServiceProvider( const Char * _name ) override;
        const bool * isAvailableServiceProvider( const Char * _name ) override;
        const bool * isInitializeServiceProvider( const Char * _name ) override;

    protected:
        const ServiceInterfacePtr & getService( const Char * _name ) const override;

    protected:
        bool createService( FServiceProviderGenerator _generator, bool _safe, const DocumentPtr & _doc ) override;
        bool finalizeService( const Char * _name ) override;

        bool destroyService( const Char * _name ) override;

    public:
        void dependencyService( const Char * _name, const Char * _dependency ) override;
        bool waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda ) override;
        bool leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda ) override;
        void unlinkService( const Char * _owner ) override;

    protected:
        void stopServices() override;

    protected:
        void destroy() override;

    protected:
        ServiceInterfacePtr generateService_( FServiceProviderGenerator _generator, const DocumentPtr & _doc );
        void removeDependency_( const ServiceInterfacePtr & _service );
        bool checkWaits_( const ServiceInterfacePtr & _service );

    protected:
        struct ServiceDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> required[16];
            uint32_t required_count;
            ServiceInterfacePtr service;
            bool exist = false;
            bool safe = false;
            bool available = false;
            bool initialize = false;
            bool requiring = false;
        };

        ServiceDesc m_services[MENGINE_SERVICE_PROVIDER_COUNT];

        uint32_t m_servicesCount;

        struct DependencyDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> dependency;
        };

        DependencyDesc m_dependencies[MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT];

        uint32_t m_dependenciesCount;

        struct LeaveDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> owner;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            LambdaLeaveService lambda;
        };

        LeaveDesc m_leaving[MENGINE_SERVICE_PROVIDER_LEAVE_COUNT];

        uint32_t m_leaveCount;

        struct WaitDesc
        {
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> owner;
            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> name;
            LambdaWaitService lambda;
        };

        WaitDesc m_waits[MENGINE_SERVICE_PROVIDER_MAX_WAIT];

        uint32_t m_waitsCount;

#ifdef MENGINE_DEBUG
        const Char * m_initializeServiceName;
#endif

    protected:
        bool isRequired_( const ServiceDesc & _desc );
        bool initializeService_( ServiceDesc & _desc, const ServiceInterfacePtr & _service, bool _safe, const DocumentPtr & _doc );
        bool checkRequired_( const DocumentPtr & _doc );
    };
}