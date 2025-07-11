#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Assertion.h"
#include "Kernel/ExceptionHelper.h"
#include "Kernel/Typename.h"
#include "Kernel/ServiceRequiredList.h"

#if defined(MENGINE_DEBUG)
#   include "Config/DynamicCast.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServiceInterface
        : public Factorable
    {
    public:
        virtual const Char * getServiceId() const = 0;

    public:
        virtual bool isAvailableService() const = 0;
        virtual bool isInitializeService() const = 0;
        virtual bool isStopService() const = 0;

    public:
        virtual const ServiceRequiredList & requiredServices() const = 0;

    protected:
        virtual bool availableService() = 0;
        virtual bool initializeService( ServiceProviderInterface * _serviceProvider ) = 0;
        virtual void finalizeService() = 0;
        virtual void replaceService() = 0;
        virtual bool runService() = 0;
        virtual void stopService() = 0;

    public:
        friend class ServiceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ServiceInterface> ServiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            T * getService2( MENGINE_DEBUG_ARGUMENTS( const Char * _file, int32_t _line, const Char * _function ) )
            {
                const Char * serviceName = T::getStaticServiceId();

                ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

#if defined(MENGINE_DEBUG)
                if( serviceProvider == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line, _function )("Service '%s' invalid get provider"
                        , serviceName
                        );

                    return nullptr;
                }
#endif

                const ServiceInterfacePtr & service = serviceProvider->getService( serviceName );

                ServiceInterface * service_ptr = service.get();

#if defined(MENGINE_DEBUG)
                if( service_ptr == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line, _function )("Service '%s' not found"
                        , serviceName
                        );

                    return nullptr;
                }
#endif

#if defined(MENGINE_DEBUG)
                if( Helper::dynamicCast<T *>( service_ptr ) == nullptr )
                {
                    const Char * typenameValue = Typename<T>::value;

                    MENGINE_THROW_EXCEPTION_FL( _file, _line, _function )("Service '%s' invalid cast to '%s'"
                        , serviceName
                        , typenameValue
                        );

                    return nullptr;
                }
#endif

                T * service_ptr_t = static_cast<T *>(service_ptr);

                return service_ptr_t;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getService( MENGINE_DEBUG_ARGUMENTS( const Char * _file, int32_t _line, const Char * _function ) )
        {
            static T * s_service = Detail::getService2<T>( MENGINE_DEBUG_ARGUMENTS( _file, _line, _function ) );

            MENGINE_ASSERTION_FATAL( s_service == nullptr || s_service->isInitializeService() == true, "service '%s' not initialized"
                , T::getStaticServiceId()
            );

            return s_service;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isExistService()
        {
            static const bool * s_exist = SERVICE_PROVIDER_GET()
                ->isExistServiceProvider( T::getStaticServiceId() );

            return *s_exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isAvailableService()
        {
            static const bool * s_available = SERVICE_PROVIDER_GET()
                ->isAvailableServiceProvider( T::getStaticServiceId() );

            return *s_available;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isInitializeService()
        {
            static const bool * s_initialize = SERVICE_PROVIDER_GET()
                ->isInitializeServiceProvider( T::getStaticServiceId() );

            return *s_initialize;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_GET( Type )\
    (Mengine::Helper::getService<Type>(MENGINE_DEBUG_ARGUMENTS(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_EXIST( Type )\
    (Mengine::Helper::isExistService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_AVAILABLE( Type )\
    (Mengine::Helper::isAvailableService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_INITIALIZE( Type )\
    (Mengine::Helper::isInitializeService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_CREATE(Name)\
    MENGINE_PP_CONCATENATE(__createMengineService, Name)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_NOMINATION(Name)\
    MENGINE_PP_CONCATENATE(__nominationMengineService, Name)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DECLARE( ID )\
    public:\
        MENGINE_INLINE static const Mengine::Char * getStaticServiceId(){ return ID; };\
        MENGINE_INLINE const Mengine::Char * getServiceId() const override { return ID; };\
    protected:
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_ID( Type )\
    Type::getStaticServiceId()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FACTORY( Name, Implement )\
    bool SERVICE_FUNCTION_CREATE(Name)(Mengine::ServiceInterface**_service){\
    if(_service==nullptr){return false;}\
    try{*_service=new Implement();}catch(...){return false;}\
    return true;}\
    const Mengine::Char * SERVICE_FUNCTION_NOMINATION(Name)(){\
    return SERVICE_ID(Implement);}\
    struct MENGINE_PP_CONCATENATE(__mengine_dummy_factory, Name){}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DEPENDENCY( Type, Dependency )\
    SERVICE_PROVIDER_GET()->dependencyService(SERVICE_ID(Type), SERVICE_ID(Dependency))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXTERN( Name )\
    extern bool SERVICE_FUNCTION_CREATE( Name )(Mengine::ServiceInterface**);\
    extern const Mengine::Char * SERVICE_FUNCTION_NOMINATION( Name )()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE( Name, Doc )\
    SERVICE_PROVIDER_GET()->createService(&SERVICE_FUNCTION_CREATE(Name), false, Doc)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE_SAFE( Name, Doc )\
    SERVICE_PROVIDER_GET()->createService(&SERVICE_FUNCTION_CREATE(Name), true, Doc)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FINALIZE( Name )\
    SERVICE_PROVIDER_GET()->finalizeService(SERVICE_FUNCTION_NOMINATION(Name)())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DESTROY( Name )\
    SERVICE_PROVIDER_GET()->destroyService(SERVICE_FUNCTION_NOMINATION(Name)())
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_WAIT( Owner, Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(#Owner, SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_LEAVE( Owner, Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(#Owner, SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_UNLINK( Owner )\
    SERVICE_PROVIDER_GET()->unlinkService(#Owner)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_WAIT( Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(this->getServiceId(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_WAIT_METHOD( Type, Self, Method )\
    SERVICE_PROVIDER_GET()->waitService(this->getServiceId(), SERVICE_ID(Type), [Self](){return Self->Method();})
//////////////////////////////////////////////////////////////////////////
#define SERVICE_LEAVE( Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(this->getServiceId(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_LEAVE_METHOD( Type, Self, Method )\
    SERVICE_PROVIDER_GET()->leaveService(this->getServiceId(), SERVICE_ID(Type), [Self](){Self->Method();})
//////////////////////////////////////////////////////////////////////////
